#include "signing_orchestrator.h"
#include "../config/mpc_config.h"
#include "../utils/hex_utils.h"
#include "../utils/serialization/eddsa_serializer.h"
#include "cosigner/asymmetric_eddsa_cosigner_client.h"
#include "cosigner/asymmetric_eddsa_cosigner_server.h"
#include "cosigner/cosigner_exception.h"
#include <iostream>
#include <set>
#include <cstring>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace mpc {

SigningOrchestrator::SigningOrchestrator(
    uint64_t node_id,
    bool is_initiator,
    network::NetworkManager& network,
    persistence::InMemoryKeyPersistency& key_persistency,
    persistence::MockClientPersistency& client_persistency,
    persistence::MockServerPersistency& server_persistency,
    platform::SimplePlatform& platform)
    : my_id(node_id), initiator(is_initiator), net(network), 
      key_pers(key_persistency), client_pers(client_persistency), 
      server_pers(server_persistency), plat(platform) {}

void SigningOrchestrator::run(const std::string& txid) {
    std::cout << "\n=== STARTING REAL EDDSA SIGNING ===\n";
    
    bool is_client = (my_id == 1);
    
    // WAIT FOR USER TO TYPE "start"
    std::cout << "\n>>> Type 'start' and press Enter to begin signing: " << std::flush;
    std::string user_input;
    std::cin >> user_input;
    
    if (user_input != "start") {
        std::cout << "Signing cancelled." << std::endl;
        return;
    }
    
    // Synchronization barrier - all nodes broadcast ready signal
    std::cout << "[Node " << my_id << "] Broadcasting ready signal..." << std::endl;
    net.broadcast("sign_" + txid + "_ready", "ready");
    
    // Wait for all 3 nodes to be ready
    std::cout << "[Node " << my_id << "] Waiting for all nodes to be ready..." << std::endl;
    net.await_data("sign_" + txid + "_ready");
    std::cout << "[Node " << my_id << "] All nodes ready! Starting signing protocol..." << std::endl;
    
    // SERVERS: Verify preprocessed data is available BEFORE starting signing
    if (!is_client) {
        std::cout << "[Server] Verifying preprocessed data before signing..." << std::endl;
        eddsa_commitment loaded_commit;
        server_pers.load_preprocessed_data(config::KEY_ID, 0, loaded_commit);
        bool is_empty = true;
        for (auto byte : loaded_commit) {
            if (byte != 0) {
                is_empty = false;
                break;
            }
        }
        if (is_empty) {
            std::cout << "[Server] ERROR: No preprocessed data found at index 0!" << std::endl;
            std::cout << "[Server] Preprocessing must be completed before signing." << std::endl;
            throw cosigner_exception(cosigner_exception::INVALID_PRESIGNING_INDEX);
        }
        std::cout << "[Server] Preprocessed data verified (commitment: " 
                  << utils::to_hex(loaded_commit.data(), 32).substr(0, 16) << "...)" << std::endl;
    }
    
    // Message to sign
    std::string message = "Hello from distributed MPC!";
    
    try {
        if (is_client) {
            run_client_signing(txid, message);
        } else {
            run_server_signing(txid, message);
        }
        std::cout << "=== SIGNING COMPLETE ===\n";
    } catch (const cosigner_exception& e) {
        std::cerr << "[ERROR] Signing failed: " << e.what() 
                  << " (error code: " << e.error_code() << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Signing failed: " << e.what() << std::endl;
    }
}

void SigningOrchestrator::run_client_signing(const std::string& txid, const std::string& message) {
    std::cout << "[Client] Signing message: " << message << std::endl;
    
    std::set<uint64_t> players_ids = {1, 2, 3};
    std::set<std::string> players_str = {"1", "2", "3"};
    
    // Prepare signing data
    signing_data sign_data;
    memset(sign_data.chaincode, 0, sizeof(HDChaincode)); // Empty chaincode
    signing_block_data block;
    block.data = std::vector<uint8_t>(message.begin(), message.end());
    block.path = {}; // Empty path
    sign_data.blocks.push_back(block);
    
    asymmetric_eddsa_cosigner_client client(plat, key_pers, client_pers);
    
    // STEP 1: Wait for server R-commitments
    std::cout << "[Client] " << std::endl;
    std::cout << "[Client] Step 1: Waiting for server R-commitments..." << std::endl;
    auto server_commits_data = net.await_data_custom_count("sign_" + txid + "_server_commit", 2);
    
    std::map<uint64_t, std::vector<eddsa_commitment>> all_commitments;
    for (auto& [pid, hex] : server_commits_data) {
        all_commitments[pid] = serialization::deserialize_commitments(hex);
        std::cout << "[Client] Received " << all_commitments[pid].size() 
                  << " commitments from server " << pid << std::endl;
    }
    
    // STEP 4: Wait for server decommitted Rs (after broadcast_r)
    std::cout << "[Client] " << std::endl;
    std::cout << "[Client] Step 4: Waiting for server Rs..." << std::endl;
    auto server_Rs_data = net.await_data_custom_count("sign_" + txid + "_server_r", 2);
    
    // Parse Rs from servers
    std::map<uint64_t, Rs_and_commitments> server_Rs;
    for (auto& [pid, hex] : server_Rs_data) {
        if (pid == my_id) continue;
        server_Rs[pid] = serialization::deserialize_Rs_and_commitments(hex);
        std::cout << "[Client] Received " << server_Rs[pid].Rs.size() 
                  << " Rs from server " << pid << std::endl;
    }
    
    // STEP 5: Generate partial signatures (client implicitly decommits here)
    std::cout << "[Client] " << std::endl;
    std::cout << "[Client] Step 5: Generating partial signatures..." << std::endl;
    std::cout << "[Client] Using preprocessed_data_index=0 with KEY_ID=" << config::KEY_ID << std::endl;
    std::vector<eddsa_signature> partial_sigs;
    client.eddsa_sign_offline(config::KEY_ID, txid, sign_data, "{}", players_str, 
                             players_ids, 0, server_Rs, partial_sigs);
    
    std::cout << "[Client] Generated " << partial_sigs.size() << " partial signature(s)" << std::endl;
    
    // Broadcast partial signatures to servers
    net.broadcast("sign_" + txid + "_r2", serialization::serialize_eddsa_signatures(partial_sigs));
    
    // Wait for final signatures from servers (only need 1 server's response)
    std::cout << "[Client] Waiting for final signatures..." << std::endl;
    auto r3_data = net.await_data_custom_count("sign_" + txid + "_r3", 2);
    
    // Get final signature from any server
    auto final_sigs = serialization::deserialize_eddsa_signatures(r3_data.begin()->second);
    
    std::cout << "\n=== SIGNATURE SUCCESS ===\n";
    std::cout << "Message: \"" << message << "\"" << std::endl;
    std::cout << "Full Signature R (32 bytes): " << utils::to_hex(final_sigs[0].R, 32) << std::endl;
    std::cout << "Full Signature s (32 bytes): " << utils::to_hex(final_sigs[0].s, 32) << std::endl;
    std::cout << "Combined Signature (64 bytes): " << utils::to_hex(final_sigs[0].R, 32) 
              << utils::to_hex(final_sigs[0].s, 32) << std::endl;
    std::cout << "=========================\n" << std::endl;
}

void SigningOrchestrator::run_server_signing(const std::string& txid, const std::string& message) {
    std::cout << "[Server] Participating in signing" << std::endl;
    
    std::set<uint64_t> players_ids = {1, 2, 3};
    std::set<std::string> players_str = {"1", "2", "3"};
    
    // Prepare signing data
    signing_data sign_data;
    memset(sign_data.chaincode, 0, sizeof(HDChaincode)); // Empty chaincode
    signing_block_data block;
    block.data = std::vector<uint8_t>(message.begin(), message.end());
    block.path = {}; // Empty path
    sign_data.blocks.push_back(block);
    
    asymmetric_eddsa_cosigner_server server(plat, key_pers, server_pers);
    
    // STEP 1: Generate server's R-commitments (client commitments already stored from preprocessing)
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 1: Generating server R-commitments..." << std::endl;
    std::cout << "[Server] Using txid for signing: " << txid << std::endl;
    std::vector<eddsa_commitment> server_R_commitments;
    Rs_and_commitments server_Rs;
    server.eddsa_sign_offline(config::KEY_ID, txid, sign_data, "{}", players_str, 
                             players_ids, 0, server_R_commitments, server_Rs);
    
    std::cout << "[Server] Generated " << server_R_commitments.size() << " commitments" << std::endl;
    
    // STEP 2: Broadcast server's R-commitments to client
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 2: Broadcasting R-commitments to client..." << std::endl;
    net.broadcast("sign_" + txid + "_server_commit", serialization::serialize_commitments(server_R_commitments));
    
    // STEP 3: Decommit phase - collect commitments from SERVERS ONLY
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 3: Collecting server commitments for decommit..." << std::endl;
    std::map<uint64_t, std::vector<eddsa_commitment>> server_commitments;
    server_commitments[my_id] = server_R_commitments; // Own commitments
    
    // Collect ALL server commitments (both servers broadcast, so wait for 2 total)
    auto all_server_commits = net.await_data_custom_count("sign_" + txid + "_server_commit", 2);
    for (auto& [pid, hex] : all_server_commits) {
        if (pid != my_id && pid != 1) { // Not self, not client
            server_commitments[pid] = serialization::deserialize_commitments(hex);
            std::cout << "[Server] Received " << server_commitments[pid].size() 
                      << " commitments from server " << pid << std::endl;
        }
    }
    
    std::cout << "[Server] Total SERVER commitments for decommit: " << server_commitments.size() << std::endl;
    
    // Decommit R - SDK will load client commitments from internal storage
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 4: Decommitting Rs (have " << server_commitments.size() 
              << " server commitments + client from storage)..." << std::endl;
    std::vector<elliptic_curve_point> decommitted_Rs;
    server.decommit_r(txid, server_commitments, decommitted_Rs);
    std::cout << "[Server] Decommitted " << decommitted_Rs.size() << " Rs" << std::endl;
    
    // STEP 4.5: Exchange decommitted Rs with other servers
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 4.5: Broadcasting my Rs to other servers..." << std::endl;
    net.broadcast("sign_" + txid + "_server_decommit_r", serialization::serialize_elliptic_curve_points(decommitted_Rs));
    
    // Wait for ALL server Rs (only 2 servers broadcast on this channel)
    auto all_server_Rs_data = net.await_data_custom_count("sign_" + txid + "_server_decommit_r", 2);
    std::map<uint64_t, std::vector<elliptic_curve_point>> all_Rs;
    all_Rs[my_id] = decommitted_Rs;
    for (auto& [pid, hex] : all_server_Rs_data) {
        if (pid != my_id && pid != 1) { // Not self, not client
            all_Rs[pid] = serialization::deserialize_elliptic_curve_points(hex);
            std::cout << "[Server] Received " << all_Rs[pid].size() 
                      << " Rs from server " << pid << std::endl;
        }
    }
    
    // STEP 6: Broadcast R phase - aggregate all Rs
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 6: Calling broadcast_r with " << all_Rs.size() 
              << " players' Rs..." << std::endl;
    uint64_t send_to;
    server.broadcast_r(txid, all_Rs, server_Rs, send_to);
    std::cout << "[Server] Broadcast Rs complete, got " << server_Rs.Rs.size() << " final Rs" << std::endl;
    
    // STEP 7: Send Rs to client
    net.broadcast("sign_" + txid + "_server_r", serialization::serialize_Rs_and_commitments(server_Rs));
    
    // Wait for client partial signatures (only client broadcasts on this channel)
    std::cout << "[Server] Waiting for client partial signatures..." << std::endl;
    auto r2_data = net.await_data_custom_count("sign_" + txid + "_r2", 1);
    auto client_partial_sigs = serialization::deserialize_eddsa_signatures(r2_data[1]);
    
    // Process partial signatures
    std::cout << "[Server] " << std::endl;
    std::cout << "[Server] Step 7: Processing partial signatures..." << std::endl;
    std::vector<eddsa_signature> sigs;
    std::set<uint64_t> send_to_set;
    bool final_signature;
    server.broadcast_si(txid, 1, config::MPC_PROTOCOL_VERSION, client_partial_sigs, 
                       sigs, send_to_set, final_signature);
    
    if (!final_signature) {
        // Need another round - servers exchange their signatures
        net.broadcast("sign_" + txid + "_r2_sigs", serialization::serialize_eddsa_signatures(sigs));
        auto sigs_data = net.await_data_custom_count("sign_" + txid + "_r2_sigs", 2); // Only 2 servers
        
        std::cout << "[Server] Receive signatures from other servers!" << std::endl;
        std::map<uint64_t, std::vector<eddsa_signature>> all_sigs;
        for (auto& [pid, hex] : sigs_data) {
            all_sigs[pid] = serialization::deserialize_eddsa_signatures(hex);
        }
        
        server.get_eddsa_signature(txid, all_sigs, sigs);
    }
    
    std::cout << "[Server] Final signature generated!" << std::endl;
    net.broadcast("sign_" + txid + "_r3", serialization::serialize_eddsa_signatures(sigs));
}

} // namespace mpc
} // namespace mpc_demo

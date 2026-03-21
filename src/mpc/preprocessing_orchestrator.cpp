#include "preprocessing_orchestrator.h"
#include "../config/mpc_config.h"
#include "../utils/serialization/eddsa_serializer.h"
#include "cosigner/asymmetric_eddsa_cosigner_client.h"
#include "cosigner/asymmetric_eddsa_cosigner_server.h"
#include <iostream>
#include <set>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace mpc {

PreprocessingOrchestrator::PreprocessingOrchestrator(
    uint64_t node_id,
    network::NetworkManager& network,
    persistence::InMemoryKeyPersistency& key_persistency,
    persistence::MockClientPersistency& client_persistency,
    persistence::MockServerPersistency& server_persistency,
    platform::SimplePlatform& platform)
    : my_id(node_id), net(network), key_pers(key_persistency), 
      client_pers(client_persistency), server_pers(server_persistency), plat(platform) {}

void PreprocessingOrchestrator::run(const std::string& request_id) {
    std::cout << "\n=== STARTING PREPROCESSING ===\n";
    
    bool is_client = (my_id == 1);
    std::set<uint64_t> players_ids = {1, 2, 3};
    uint32_t count = config::PREPROCESSING_COUNT;
    
    if (is_client) {
        // Client generates R-commitments for preprocessing
        asymmetric_eddsa_cosigner_client client(plat, key_pers, client_pers);
        
        std::cout << "[Client] Generating " << count << " R-commitments for preprocessing..." << std::endl;
        std::vector<eddsa_commitment> R_commitments;
        client.start_signature_preprocessing(config::TENANT_ID, config::KEY_ID, request_id, 
                                            0, count, count, players_ids, R_commitments);
        
        std::cout << "[Client] Generated " << R_commitments.size() << " R-commitments" << std::endl;
        
        // Broadcast to servers
        net.broadcast("preprocess_" + request_id, serialization::serialize_commitments(R_commitments));
        std::cout << "[Client] Broadcast R-commitments to servers" << std::endl;
        
    } else {
        // Servers wait for client's R-commitments and store them
        std::cout << "[Server] Waiting for client R-commitments..." << std::endl;
        auto client_commit_data = net.await_data_custom_count("preprocess_" + request_id, 1);
        auto client_commitments = serialization::deserialize_commitments(client_commit_data[1]);
        
        std::cout << "[Server] Received " << client_commitments.size() << " R-commitments from client" << std::endl;
        
        // Store for later use
        asymmetric_eddsa_cosigner_server server(plat, key_pers, server_pers);
        std::cout << "[Server] Storing preprocessing with request_id: " << request_id << std::endl;
        server.store_presigning_data(config::KEY_ID, request_id, 0, count, count, players_ids, 1, client_commitments);
        
        std::cout << "[Server] Stored client R-commitments for request_id: " << request_id << std::endl;
    }
    
    std::cout << "=== PREPROCESSING COMPLETE ===\n";
}

} // namespace mpc
} // namespace mpc_demo

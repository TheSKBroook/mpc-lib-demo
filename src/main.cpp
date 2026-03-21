#include "config/mpc_config.h"
#include "network/network_manager.h"
#include "persistence/in_memory_key_persistency.h"
#include "persistence/mock_client_persistency.h"
#include "persistence/mock_server_persistency.h"
#include "platform/simple_platform.h"
#include "mpc/keygen_orchestrator.h"
#include "mpc/preprocessing_orchestrator.h"
#include "mpc/signing_orchestrator.h"
#include <iostream>
#include <chrono>

using namespace mpc_demo;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <node_id>" << std::endl;
        return 1;
    }
    
    uint64_t my_id = std::stoull(argv[1]);

    // Initialize all components
    persistence::InMemoryKeyPersistency key_persistency;
    persistence::MockClientPersistency client_persistency;
    persistence::MockServerPersistency server_persistency;
    platform::SimplePlatform platform(my_id);
    network::NetworkManager network(my_id);

    // 1. Run Key Generation
    mpc::KeygenOrchestrator keygen(my_id, network, key_persistency, platform);
    keygen.run();

    // 2. Interactive Mode
    while (true) {
        std::cout << "\n[1] Start New Signing Session\n[2] Join Signing Session\n> ";
        int choice;
        std::cin >> choice;

        std::string txid;
        bool is_initiator = false;
        
        if (choice == 1) {
            txid = "tx-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            std::cout << "Created signing session: " << txid << std::endl;
            std::cout << "Tell other nodes to join with this ID!\n";
            is_initiator = true;
        } else {
            std::cout << "Enter Transaction ID: ";
            std::cin >> txid;
        }
        
        // Run Preprocessing (generate and distribute nonces)
        std::cout << "\n=== Running preprocessing for session " << txid << " ===\n";
        mpc::PreprocessingOrchestrator preprocessing(my_id, network, key_persistency, 
                                                     client_persistency, server_persistency, platform);
        preprocessing.run(txid);
        
        // Run Signing
        mpc::SigningOrchestrator signing(my_id, is_initiator, network, key_persistency, 
                                        client_persistency, server_persistency, platform);
        signing.run(txid);
    }
    
    return 0;
}

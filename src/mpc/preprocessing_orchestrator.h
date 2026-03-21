#pragma once

#include "../network/network_manager.h"
#include "../persistence/in_memory_key_persistency.h"
#include "../persistence/mock_client_persistency.h"
#include "../persistence/mock_server_persistency.h"
#include "../platform/simple_platform.h"
#include <string>
#include <cstdint>

namespace mpc_demo {
namespace mpc {

/**
 * Orchestrates the MPC preprocessing protocol
 */
class PreprocessingOrchestrator {
public:
    PreprocessingOrchestrator(uint64_t node_id,
                             network::NetworkManager& network,
                             persistence::InMemoryKeyPersistency& key_persistency,
                             persistence::MockClientPersistency& client_persistency,
                             persistence::MockServerPersistency& server_persistency,
                             platform::SimplePlatform& platform);

    /**
     * Run the complete preprocessing protocol
     */
    void run(const std::string& request_id);

private:
    uint64_t my_id;
    network::NetworkManager& net;
    persistence::InMemoryKeyPersistency& key_pers;
    persistence::MockClientPersistency& client_pers;
    persistence::MockServerPersistency& server_pers;
    platform::SimplePlatform& plat;
};

} // namespace mpc
} // namespace mpc_demo

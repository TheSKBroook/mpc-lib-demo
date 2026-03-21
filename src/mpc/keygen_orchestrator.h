#pragma once

#include "../network/network_manager.h"
#include "../persistence/in_memory_key_persistency.h"
#include "../platform/simple_platform.h"
#include <cstdint>

namespace mpc_demo {
namespace mpc {

/**
 * Orchestrates the MPC key generation protocol
 */
class KeygenOrchestrator {
public:
    KeygenOrchestrator(uint64_t node_id, 
                       network::NetworkManager& network,
                       persistence::InMemoryKeyPersistency& persistency,
                       platform::SimplePlatform& platform);

    /**
     * Run the complete key generation protocol
     */
    void run();

private:
    uint64_t my_id;
    network::NetworkManager& net;
    persistence::InMemoryKeyPersistency& pers;
    platform::SimplePlatform& plat;
};

} // namespace mpc
} // namespace mpc_demo

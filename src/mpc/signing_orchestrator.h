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
 * Orchestrates the MPC signing protocol
 */
class SigningOrchestrator {
public:
    SigningOrchestrator(uint64_t node_id,
                       bool is_initiator,
                       network::NetworkManager& network,
                       persistence::InMemoryKeyPersistency& key_persistency,
                       persistence::MockClientPersistency& client_persistency,
                       persistence::MockServerPersistency& server_persistency,
                       platform::SimplePlatform& platform);

    /**
     * Run the complete signing protocol
     */
    void run(const std::string& txid);

private:
    void run_client_signing(const std::string& txid, const std::string& message);
    void run_server_signing(const std::string& txid, const std::string& message);

    uint64_t my_id;
    bool initiator;
    network::NetworkManager& net;
    persistence::InMemoryKeyPersistency& key_pers;
    persistence::MockClientPersistency& client_pers;
    persistence::MockServerPersistency& server_pers;
    platform::SimplePlatform& plat;
};

} // namespace mpc
} // namespace mpc_demo

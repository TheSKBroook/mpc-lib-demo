#pragma once

#include <string>
#include <map>
#include <cstdint>

// Forward declaration - CURL is defined in curl.h
struct CURL;

namespace mpc_demo {
namespace network {

/**
 * Manages network communication between MPC nodes via HTTP relay server
 */
class NetworkManager {
public:
    explicit NetworkManager(uint64_t node_id);
    ~NetworkManager();

    /**
     * Broadcast data to all nodes in a given round
     */
    void broadcast(const std::string& round_id, const std::string& data);

    /**
     * Wait for data from all nodes in a round (using configured TOTAL_PLAYERS)
     */
    std::map<uint64_t, std::string> await_data(const std::string& round_id);

    /**
     * Wait for data from a specific number of nodes in a round
     */
    std::map<uint64_t, std::string> await_data_custom_count(const std::string& round_id, int expected_count);

private:
    uint64_t my_id;
    CURL* curl;

    /**
     * Parse JSON response to extract player data map
     */
    std::map<uint64_t, std::string> parse_json_map(const std::string& json);
};

} // namespace network
} // namespace mpc_demo

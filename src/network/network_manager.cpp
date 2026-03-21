#include "network_manager.h"
#include "../config/mpc_config.h"
#include <curl/curl.h>
#include <iostream>
#include <thread>
#include <chrono>

namespace mpc_demo {
namespace network {

// CURL callback for writing response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

NetworkManager::NetworkManager(uint64_t node_id) : my_id(node_id) {
    curl = curl_easy_init();
}

NetworkManager::~NetworkManager() {
    curl_easy_cleanup(curl);
}

void NetworkManager::broadcast(const std::string& round_id, const std::string& data) {
    std::string url = config::RELAY_URL + "/broadcast";
    std::string json = "{\"sender_id\": " + std::to_string(my_id) + 
                       ", \"round_id\": \"" + round_id + 
                       "\", \"data\": \"" + data + "\"}";
    
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    std::string resp;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_perform(curl);
    curl_slist_free_all(headers);
    std::cout << "[NET] Sent Round " << round_id << std::endl;
}

std::map<uint64_t, std::string> NetworkManager::await_data(const std::string& round_id) {
    return await_data_custom_count(round_id, config::TOTAL_PLAYERS);
}

std::map<uint64_t, std::string> NetworkManager::await_data_custom_count(const std::string& round_id, int expected_count) {
    std::string url = config::RELAY_URL + "/poll?round_id=" + round_id + 
                      "&expected_count=" + std::to_string(expected_count);
    
    std::cout << "[NET] Waiting " << round_id;
    if (expected_count != config::TOTAL_PLAYERS) {
        std::cout << " (expecting " << expected_count << ")";
    }
    std::cout << "..." << std::flush;
    
    while (true) {
        std::string resp;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
        curl_easy_perform(curl);

        if (resp.find("\"ready\":true") != std::string::npos) {
            std::cout << " OK!" << std::endl;
            return parse_json_map(resp);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

std::map<uint64_t, std::string> NetworkManager::parse_json_map(const std::string& json) {
    std::map<uint64_t, std::string> res;
    size_t data_start = json.find("\"data\"");
    if (data_start == std::string::npos) return res;
    
    size_t brace_pos = json.find("{", data_start);
    if (brace_pos == std::string::npos) return res;
    
    for(int i = 1; i <= config::TOTAL_PLAYERS; ++i) {
        std::string pattern = "\"" + std::to_string(i) + "\":";
        size_t key_pos = json.find(pattern, brace_pos);
        if(key_pos == std::string::npos) continue;
        
        size_t value_start = json.find("\"", key_pos + pattern.length());
        if (value_start == std::string::npos) continue;
        value_start++;
        
        size_t value_end = json.find("\"", value_start);
        if (value_end == std::string::npos) continue;
        
        res[i] = json.substr(value_start, value_end - value_start);
        brace_pos = value_end;
    }
    return res;
}

} // namespace network
} // namespace mpc_demo

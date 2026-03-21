#pragma once

#include <string>
#include <cstdint>

namespace mpc_demo {
namespace config {

// Network Configuration
static const std::string RELAY_URL = "http://127.0.0.1:5000";

// MPC Configuration
static const std::string TENANT_ID = "DEMO_TENANT";
static const std::string KEY_ID = "demo-key-1";
static const int TOTAL_PLAYERS = 3;
static const uint32_t MPC_PROTOCOL_VERSION = 1;

// Preprocessing Configuration
static const uint32_t PREPROCESSING_COUNT = 1000;

} // namespace config
} // namespace mpc_demo

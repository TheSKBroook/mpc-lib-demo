#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace mpc_demo {
namespace utils {

/**
 * Convert raw bytes to hexadecimal string
 */
std::string to_hex(const uint8_t* data, size_t len);

/**
 * Convert vector of bytes to hexadecimal string
 */
std::string to_hex(const std::vector<uint8_t>& data);

/**
 * Convert hexadecimal string to vector of bytes
 */
std::vector<uint8_t> from_hex(const std::string& hex);

} // namespace utils
} // namespace mpc_demo

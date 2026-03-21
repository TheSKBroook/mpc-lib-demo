#include "hex_utils.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace mpc_demo {
namespace utils {

std::string to_hex(const uint8_t* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << (int)data[i];
    }
    return ss.str();
}

std::string to_hex(const std::vector<uint8_t>& data) {
    return to_hex(data.data(), data.size());
}

std::vector<uint8_t> from_hex(const std::string& hex) {
    std::vector<uint8_t> data;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        data.push_back((uint8_t)strtol(byteString.c_str(), nullptr, 16));
    }
    return data;
}

} // namespace utils
} // namespace mpc_demo

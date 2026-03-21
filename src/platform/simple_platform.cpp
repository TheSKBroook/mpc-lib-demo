#include "simple_platform.h"
#include "../config/mpc_config.h"
#include <random>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace platform {

SimplePlatform::SimplePlatform(uint64_t node_id) : my_id(node_id) {}

uint64_t SimplePlatform::get_id_from_keyid(const std::string&) const {
    return my_id;
}

const std::string SimplePlatform::get_current_tenantid() const {
    return config::TENANT_ID;
}

void SimplePlatform::gen_random(size_t len, uint8_t* out) const {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    for (size_t i = 0; i < len; ++i) {
        out[i] = dist(rng);
    }
}

uint64_t SimplePlatform::now_msec() const {
    return 0;
}

byte_vector_t SimplePlatform::encrypt_for_player(uint64_t, const byte_vector_t& d) const {
    return d;  // No encryption in this demo
}

byte_vector_t SimplePlatform::decrypt_message(const byte_vector_t& d) const {
    return d;  // No decryption in this demo
}

void SimplePlatform::derive_initial_share(const share_derivation_args&, 
                                          cosigner_sign_algorithm, 
                                          elliptic_curve256_scalar_t*) const {
    // Not used in this demo
}

bool SimplePlatform::backup_key(const std::string&, cosigner_sign_algorithm, 
                                const elliptic_curve256_scalar_t&, 
                                const cmp_key_metadata&, 
                                const auxiliary_keys&) {
    return true;  // Always succeed in demo
}

void SimplePlatform::on_start_signing(const std::string&, const std::string&, 
                                      const signing_data&, 
                                      const std::string&, const std::set<std::string>&, 
                                      const platform_service::signing_type) {
    // No-op in this demo
}

void SimplePlatform::fill_signing_info_from_metadata(const std::string&, std::vector<uint32_t>&) const {
    // No-op in this demo
}

bool SimplePlatform::is_client_id(uint64_t player_id) const {
    return player_id == 1;  // Player 1 is the client
}

} // namespace platform
} // namespace mpc_demo

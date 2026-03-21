#pragma once

#include "cosigner/platform_service.h"
#include <cstdint>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace platform {

/**
 * Simple implementation of platform service for MPC operations
 */
class SimplePlatform : public fireblocks::common::cosigner::platform_service {
public:
    explicit SimplePlatform(uint64_t node_id);
    ~SimplePlatform() override = default;

    uint64_t get_id_from_keyid(const std::string&) const override;
    const std::string get_current_tenantid() const override;
    void gen_random(size_t len, uint8_t* out) const override;
    uint64_t now_msec() const override;
    fireblocks::common::cosigner::byte_vector_t encrypt_for_player(
        uint64_t, const fireblocks::common::cosigner::byte_vector_t& d) const override;
    fireblocks::common::cosigner::byte_vector_t decrypt_message(
        const fireblocks::common::cosigner::byte_vector_t& d) const override;
    void derive_initial_share(const fireblocks::common::cosigner::share_derivation_args&, 
                             fireblocks::common::cosigner::cosigner_sign_algorithm, 
                             fireblocks::common::cosigner::elliptic_curve256_scalar_t*) const override;
    bool backup_key(const std::string&, fireblocks::common::cosigner::cosigner_sign_algorithm, 
                   const fireblocks::common::cosigner::elliptic_curve256_scalar_t&, 
                   const fireblocks::common::cosigner::cmp_key_metadata&, 
                   const fireblocks::common::cosigner::auxiliary_keys&) override;
    void on_start_signing(const std::string&, const std::string&, 
                         const fireblocks::common::cosigner::signing_data&, 
                         const std::string&, const std::set<std::string>&, 
                         const fireblocks::common::cosigner::platform_service::signing_type) override;
    void fill_signing_info_from_metadata(const std::string&, std::vector<uint32_t>&) const override;
    bool is_client_id(uint64_t player_id) const override;

private:
    uint64_t my_id;
};

} // namespace platform
} // namespace mpc_demo

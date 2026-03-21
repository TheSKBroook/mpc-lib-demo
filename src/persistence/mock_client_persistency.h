#pragma once

#include "cosigner/asymmetric_eddsa_cosigner_client.h"
#include <map>
#include <string>
#include <array>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace persistence {

/**
 * Mock implementation of client preprocessing data persistence
 */
class MockClientPersistency : public fireblocks::common::cosigner::asymmetric_eddsa_cosigner_client::preprocessing_persistency {
public:
    MockClientPersistency() = default;
    ~MockClientPersistency() override = default;

    void create_preprocessed_data(const std::string& key_id, uint64_t size) override;
    void store_preprocessed_data(const std::string& key_id, uint64_t index, 
                                 const fireblocks::common::cosigner::ed25519_scalar_t& k) override;
    void load_preprocessed_data(const std::string& key_id, uint64_t index, 
                                fireblocks::common::cosigner::ed25519_scalar_t& k) override;
    void delete_preprocessed_data(const std::string& key_id) override;

private:
    std::map<std::string, std::map<uint64_t, std::array<uint8_t, 32>>> stored_nonces;
};

} // namespace persistence
} // namespace mpc_demo

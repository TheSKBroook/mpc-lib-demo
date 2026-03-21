#pragma once

#include "cosigner/asymmetric_eddsa_cosigner_server.h"
#include <map>
#include <string>
#include <vector>

namespace mpc_demo {
namespace persistence {

/**
 * Mock implementation of server signing data persistence
 */
class MockServerPersistency : public fireblocks::common::cosigner::asymmetric_eddsa_cosigner_server::signing_persistency {
public:
    MockServerPersistency() = default;
    ~MockServerPersistency() override = default;

    void create_preprocessed_data(const std::string& key_id, uint64_t size) override;
    void store_preprocessed_data(const std::string& key_id, uint64_t index, 
                                 const fireblocks::common::cosigner::eddsa_commitment& R_commitment) override;
    void load_preprocessed_data(const std::string& key_id, uint64_t index, 
                                fireblocks::common::cosigner::eddsa_commitment& R_commitment) override;
    void delete_preprocessed_data(const std::string& key_id) override;
    void store_commitments(const std::string& txid, 
                          const std::map<uint64_t, std::vector<fireblocks::common::cosigner::eddsa_commitment>>& comms) override;
    void load_commitments(const std::string& txid, 
                         std::map<uint64_t, std::vector<fireblocks::common::cosigner::eddsa_commitment>>& comms) override;
    void delete_commitments(const std::string& txid) override;
    void store_signing_data(const std::string& txid, 
                           const fireblocks::common::cosigner::asymmetric_eddsa_signing_metadata& data, 
                           bool update) override;
    void load_signing_data(const std::string& txid, 
                          fireblocks::common::cosigner::asymmetric_eddsa_signing_metadata& data) override;
    void delete_temporary_signing_data(const std::string& txid) override;

private:
    std::map<std::string, fireblocks::common::cosigner::asymmetric_eddsa_signing_metadata> signing_data;
    std::map<std::string, std::map<uint64_t, std::vector<fireblocks::common::cosigner::eddsa_commitment>>> commitments;
    std::map<std::string, std::map<uint64_t, fireblocks::common::cosigner::eddsa_commitment>> preprocessed_data;
};

} // namespace persistence
} // namespace mpc_demo

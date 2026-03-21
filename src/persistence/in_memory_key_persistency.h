#pragma once

#include "cosigner/cmp_setup_service.h"
#include <map>
#include <string>
#include <vector>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace persistence {

/**
 * In-memory implementation of key persistency for MPC key storage
 */
class InMemoryKeyPersistency : public fireblocks::common::cosigner::cmp_setup_service::setup_key_persistency {
public:
    InMemoryKeyPersistency() = default;
    ~InMemoryKeyPersistency() override = default;

    // cmp_key_persistency interface
    bool key_exist(const std::string& id) const override;
    void load_key(const std::string& id, fireblocks::common::cosigner::cosigner_sign_algorithm& algo, 
                  fireblocks::common::cosigner::elliptic_curve256_scalar_t& key) const override;
    const std::string get_tenantid_from_keyid(const std::string&) const override;
    void load_key_metadata(const std::string& id, fireblocks::common::cosigner::cmp_key_metadata& m, bool) const override;
    void load_auxiliary_keys(const std::string& id, fireblocks::common::cosigner::auxiliary_keys& a) const override;

    // setup_key_persistency interface
    void store_key(const std::string& id, fireblocks::common::cosigner::cosigner_sign_algorithm, 
                   const fireblocks::common::cosigner::elliptic_curve256_scalar_t& key, uint64_t) override;
    void store_key_metadata(const std::string& id, const fireblocks::common::cosigner::cmp_key_metadata& m, bool) override;
    void store_auxiliary_keys(const std::string& id, const fireblocks::common::cosigner::auxiliary_keys& a) override;
    void store_keyid_tenant_id(const std::string&, const std::string&) override;
    void store_setup_data(const std::string& id, const fireblocks::common::cosigner::setup_data& data) override;
    void load_setup_data(const std::string& id, fireblocks::common::cosigner::setup_data& data) override;
    void store_setup_commitments(const std::string& id, const std::map<uint64_t, fireblocks::common::cosigner::commitment>& commitments) override;
    void load_setup_commitments(const std::string& id, std::map<uint64_t, fireblocks::common::cosigner::commitment>& commitments) override;
    void delete_temporary_key_data(const std::string& id, bool) override;

private:
    std::map<std::string, std::vector<uint8_t>> keys;
    std::map<std::string, fireblocks::common::cosigner::cmp_key_metadata> metas;
    std::map<std::string, fireblocks::common::cosigner::auxiliary_keys> auxs;
    std::map<std::string, fireblocks::common::cosigner::setup_data> setup_datas;
    std::map<std::string, std::map<uint64_t, fireblocks::common::cosigner::commitment>> setup_comms;
};

} // namespace persistence
} // namespace mpc_demo

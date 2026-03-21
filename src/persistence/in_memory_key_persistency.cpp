#include "in_memory_key_persistency.h"
#include "../config/mpc_config.h"
#include "cosigner/cosigner_exception.h"
#include <cstring>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace persistence {

bool InMemoryKeyPersistency::key_exist(const std::string& id) const {
    return keys.count(id) > 0;
}

void InMemoryKeyPersistency::load_key(const std::string& id, cosigner_sign_algorithm& algo, 
                                      elliptic_curve256_scalar_t& key) const {
    if (!key_exist(id)) {
        throw cosigner_exception(cosigner_exception::BAD_KEY);
    }
    algo = EDDSA_ED25519;  // EdDSA algorithm
    memcpy(key, keys.at(id).data(), 32);
}

const std::string InMemoryKeyPersistency::get_tenantid_from_keyid(const std::string&) const {
    return config::TENANT_ID;
}

void InMemoryKeyPersistency::load_key_metadata(const std::string& id, cmp_key_metadata& m, bool) const {
    if (metas.count(id) == 0) {
        throw cosigner_exception(cosigner_exception::BAD_KEY);
    }
    m = metas.at(id);
}

void InMemoryKeyPersistency::load_auxiliary_keys(const std::string& id, auxiliary_keys& a) const {
    if (auxs.count(id) == 0) {
        throw cosigner_exception(cosigner_exception::BAD_KEY);
    }
    a = auxs.at(id);
}

void InMemoryKeyPersistency::store_key(const std::string& id, cosigner_sign_algorithm, 
                                       const elliptic_curve256_scalar_t& key, uint64_t) {
    std::vector<uint8_t> k(32);
    memcpy(k.data(), key, 32);
    keys[id] = k;
}

void InMemoryKeyPersistency::store_key_metadata(const std::string& id, const cmp_key_metadata& m, bool) {
    metas[id] = m;
}

void InMemoryKeyPersistency::store_auxiliary_keys(const std::string& id, const auxiliary_keys& a) {
    auxs[id] = a;
}

void InMemoryKeyPersistency::store_keyid_tenant_id(const std::string&, const std::string&) {
    // No-op for in-memory implementation
}

void InMemoryKeyPersistency::store_setup_data(const std::string& id, const setup_data& data) {
    setup_datas[id] = data;
}

void InMemoryKeyPersistency::load_setup_data(const std::string& id, setup_data& data) {
    if (setup_datas.count(id) == 0) {
        throw cosigner_exception(cosigner_exception::BAD_KEY);
    }
    data = setup_datas.at(id);
}

void InMemoryKeyPersistency::store_setup_commitments(const std::string& id, 
                                                     const std::map<uint64_t, commitment>& commitments) {
    setup_comms[id] = commitments;
}

void InMemoryKeyPersistency::load_setup_commitments(const std::string& id, 
                                                    std::map<uint64_t, commitment>& commitments) {
    if (setup_comms.count(id) == 0) {
        throw cosigner_exception(cosigner_exception::BAD_KEY);
    }
    commitments = setup_comms.at(id);
}

void InMemoryKeyPersistency::delete_temporary_key_data(const std::string& id, bool) {
    setup_datas.erase(id);
    setup_comms.erase(id);
}

} // namespace persistence
} // namespace mpc_demo

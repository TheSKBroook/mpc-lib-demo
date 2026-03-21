#include "mock_server_persistency.h"
#include "cosigner/cosigner_exception.h"
#include <cstring>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace persistence {

void MockServerPersistency::create_preprocessed_data(const std::string& key_id, uint64_t size) {
    preprocessed_data[key_id].clear();
}

void MockServerPersistency::store_preprocessed_data(const std::string& key_id, uint64_t index, 
                                                    const eddsa_commitment& R_commitment) {
    preprocessed_data[key_id][index] = R_commitment;
}

void MockServerPersistency::load_preprocessed_data(const std::string& key_id, uint64_t index, 
                                                   eddsa_commitment& R_commitment) {
    if (preprocessed_data.count(key_id) && preprocessed_data[key_id].count(index)) {
        R_commitment = preprocessed_data[key_id][index];
    } else {
        // No preprocessed data - return zeros
        memset(R_commitment.data(), 0, 32);
    }
}

void MockServerPersistency::delete_preprocessed_data(const std::string& key_id) {
    preprocessed_data.erase(key_id);
}

void MockServerPersistency::store_commitments(const std::string& txid, 
                                              const std::map<uint64_t, std::vector<eddsa_commitment>>& comms) {
    commitments[txid] = comms;
}

void MockServerPersistency::load_commitments(const std::string& txid, 
                                             std::map<uint64_t, std::vector<eddsa_commitment>>& comms) {
    if (commitments.count(txid)) {
        comms = commitments[txid];
    } else {
        throw cosigner_exception(cosigner_exception::INVALID_TRANSACTION);
    }
}

void MockServerPersistency::delete_commitments(const std::string& txid) {
    commitments.erase(txid);
}

void MockServerPersistency::store_signing_data(const std::string& txid, 
                                               const asymmetric_eddsa_signing_metadata& data, 
                                               bool update) {
    signing_data[txid] = data;
}

void MockServerPersistency::load_signing_data(const std::string& txid, 
                                              asymmetric_eddsa_signing_metadata& data) {
    if (signing_data.count(txid)) {
        data = signing_data[txid];
    } else {
        throw cosigner_exception(cosigner_exception::INVALID_TRANSACTION);
    }
}

void MockServerPersistency::delete_temporary_signing_data(const std::string& txid) {
    signing_data.erase(txid);
}

} // namespace persistence
} // namespace mpc_demo

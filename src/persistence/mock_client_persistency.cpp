#include "mock_client_persistency.h"
#include "cosigner/cosigner_exception.h"
#include <cstring>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace persistence {

void MockClientPersistency::create_preprocessed_data(const std::string& key_id, uint64_t size) {
    stored_nonces[key_id].clear();
}

void MockClientPersistency::store_preprocessed_data(const std::string& key_id, uint64_t index, 
                                                    const ed25519_scalar_t& k) {
    std::array<uint8_t, 32> nonce;
    memcpy(nonce.data(), k, 32);
    stored_nonces[key_id][index] = nonce;
}

void MockClientPersistency::load_preprocessed_data(const std::string& key_id, uint64_t index, 
                                                   ed25519_scalar_t& k) {
    if (stored_nonces.count(key_id) && stored_nonces[key_id].count(index)) {
        memcpy(k, stored_nonces[key_id][index].data(), 32);
    } else {
        // No preprocessed data available - this should not happen!
        throw cosigner_exception(cosigner_exception::INVALID_PRESIGNING_INDEX);
    }
}

void MockClientPersistency::delete_preprocessed_data(const std::string& key_id) {
    stored_nonces.erase(key_id);
}

} // namespace persistence
} // namespace mpc_demo

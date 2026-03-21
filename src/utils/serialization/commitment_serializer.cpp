#include "commitment_serializer.h"
#include "../hex_utils.h"
#include <cstring>
#include <stdexcept>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace serialization {

std::string serialize_commitment(const commitment& comm) {
    std::vector<uint8_t> buffer(64); // 32 bytes salt + 32 bytes commitment
    memcpy(buffer.data(), comm.data.salt, 32);
    memcpy(buffer.data() + 32, comm.data.commitment, 32);
    return utils::to_hex(buffer);
}

commitment deserialize_commitment(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    if (buffer.size() != 64) throw std::runtime_error("Invalid commitment size");
    commitment comm;
    memcpy(comm.data.salt, buffer.data(), 32);
    memcpy(comm.data.commitment, buffer.data() + 32, 32);
    return comm;
}

std::string serialize_decommitment(const setup_decommitment& decomm) {
    std::vector<uint8_t> buffer;
    
    // 1. ack
    buffer.insert(buffer.end(), decomm.ack, decomm.ack + sizeof(decomm.ack));
    
    // 2. seed
    buffer.insert(buffer.end(), decomm.seed, decomm.seed + sizeof(decomm.seed));
    
    // 3. share.X (Use sizeof!)
    // If the SDK stores compressed points (33 bytes), this will now be correct.
    size_t x_len = sizeof(decomm.share.X.data); 
    buffer.insert(buffer.end(), decomm.share.X.data, decomm.share.X.data + x_len);
    
    // 4. share.schnorr_R (Use sizeof!)
    size_t r_len = sizeof(decomm.share.schnorr_R.data);
    buffer.insert(buffer.end(), decomm.share.schnorr_R.data, decomm.share.schnorr_R.data + r_len);
    
    // 5. paillier
    uint32_t paillier_len = decomm.paillier_public_key.size();
    buffer.insert(buffer.end(), (uint8_t*)&paillier_len, (uint8_t*)&paillier_len + 4);
    buffer.insert(buffer.end(), decomm.paillier_public_key.begin(), decomm.paillier_public_key.end());
    
    // 6. ring pedersen
    uint32_t ring_len = decomm.ring_pedersen_public_key.size();
    buffer.insert(buffer.end(), (uint8_t*)&ring_len, (uint8_t*)&ring_len + 4);
    buffer.insert(buffer.end(), decomm.ring_pedersen_public_key.begin(), decomm.ring_pedersen_public_key.end());
    
    return utils::to_hex(buffer);
}

setup_decommitment deserialize_decommitment(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    setup_decommitment decomm;
    size_t offset = 0;

    // Helper macro to prevent overflow
    #define CHECK_BOUNDS(len) if (offset + (len) > buffer.size()) throw std::runtime_error("Buffer underflow");

    // 1. ack
    size_t ack_len = sizeof(decomm.ack);
    CHECK_BOUNDS(ack_len);
    memcpy(decomm.ack, buffer.data() + offset, ack_len);
    offset += ack_len;

    // 2. seed
    size_t seed_len = sizeof(decomm.seed);
    CHECK_BOUNDS(seed_len);
    memcpy(decomm.seed, buffer.data() + offset, seed_len);
    offset += seed_len;

    // 3. share.X
    size_t x_len = sizeof(decomm.share.X.data);
    CHECK_BOUNDS(x_len);
    memcpy(decomm.share.X.data, buffer.data() + offset, x_len);
    offset += x_len;
    
    // 4. share.schnorr_R
    size_t r_len = sizeof(decomm.share.schnorr_R.data);
    CHECK_BOUNDS(r_len);
    memcpy(decomm.share.schnorr_R.data, buffer.data() + offset, r_len);
    offset += r_len;

    // 5. paillier
    CHECK_BOUNDS(4);
    uint32_t paillier_len;
    memcpy(&paillier_len, buffer.data() + offset, 4);
    offset += 4;
    
    CHECK_BOUNDS(paillier_len);
    decomm.paillier_public_key.assign(buffer.begin() + offset, buffer.begin() + offset + paillier_len);
    offset += paillier_len;

    // 6. ring pedersen
    CHECK_BOUNDS(4);
    uint32_t ring_len;
    memcpy(&ring_len, buffer.data() + offset, 4);
    offset += 4;

    CHECK_BOUNDS(ring_len);
    decomm.ring_pedersen_public_key.assign(buffer.begin() + offset, buffer.begin() + offset + ring_len);
    // offset += ring_len; // Not strictly needed at end, but good practice

    #undef CHECK_BOUNDS
    
    return decomm;
}

} // namespace serialization
} // namespace mpc_demo

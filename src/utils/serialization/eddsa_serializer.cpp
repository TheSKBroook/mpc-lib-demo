#include "eddsa_serializer.h"
#include "../hex_utils.h"
#include <cstring>

using namespace fireblocks::common::cosigner;

namespace mpc_demo {
namespace serialization {

std::string serialize_eddsa_commitments(const std::vector<eddsa_commitment>& comms) {
    std::vector<uint8_t> buffer;
    uint32_t count = comms.size();
    buffer.insert(buffer.end(), (uint8_t*)&count, (uint8_t*)&count + 4);
    for (const auto& comm : comms) {
        buffer.insert(buffer.end(), comm.begin(), comm.end());
    }
    return utils::to_hex(buffer);
}

std::vector<eddsa_commitment> deserialize_eddsa_commitments(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    size_t offset = 0;
    uint32_t count;
    memcpy(&count, buffer.data() + offset, 4);
    offset += 4;
    
    std::vector<eddsa_commitment> comms;
    for (uint32_t i = 0; i < count; i++) {
        eddsa_commitment comm;
        memcpy(comm.data(), buffer.data() + offset, 32);
        offset += 32;
        comms.push_back(comm);
    }
    return comms;
}

std::string serialize_elliptic_curve_points(const std::vector<elliptic_curve_point>& points) {
    std::vector<uint8_t> buffer;
    uint32_t count = points.size();
    buffer.insert(buffer.end(), (uint8_t*)&count, (uint8_t*)&count + 4);
    for (const auto& point : points) {
        buffer.insert(buffer.end(), point.data, point.data + 33);  // 33 bytes for compressed point
    }
    return utils::to_hex(buffer);
}

std::vector<elliptic_curve_point> deserialize_elliptic_curve_points(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    size_t offset = 0;
    uint32_t count;
    memcpy(&count, buffer.data() + offset, 4);
    offset += 4;
    
    std::vector<elliptic_curve_point> points;
    for (uint32_t i = 0; i < count; i++) {
        elliptic_curve_point point;
        memcpy(point.data, buffer.data() + offset, 33);  // 33 bytes for compressed point
        offset += 33;
        points.push_back(point);
    }
    return points;
}

std::string serialize_Rs_and_commitments(const Rs_and_commitments& rs) {
    std::vector<uint8_t> buffer;
    
    // Rs vector
    uint32_t rs_count = rs.Rs.size();
    buffer.insert(buffer.end(), (uint8_t*)&rs_count, (uint8_t*)&rs_count + 4);
    for (const auto& point : rs.Rs) {
        buffer.insert(buffer.end(), point.data, point.data + 33);  // 33 bytes for compressed point
    }
    
    // R_commitment (single commitment, 32 bytes)
    buffer.insert(buffer.end(), rs.R_commitment.begin(), rs.R_commitment.end());
    
    return utils::to_hex(buffer);
}

Rs_and_commitments deserialize_Rs_and_commitments(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    size_t offset = 0;
    Rs_and_commitments rs;
    
    // Rs vector
    uint32_t rs_count;
    memcpy(&rs_count, buffer.data() + offset, 4);
    offset += 4;
    for (uint32_t i = 0; i < rs_count; i++) {
        elliptic_curve_point point;
        memcpy(point.data, buffer.data() + offset, 33);  // 33 bytes for compressed point
        offset += 33;
        rs.Rs.push_back(point);
    }
    
    // R_commitment (single commitment, 32 bytes)
    memcpy(rs.R_commitment.data(), buffer.data() + offset, 32);
    
    return rs;
}

std::string serialize_commitments(const std::vector<eddsa_commitment>& commitments) {
    std::vector<uint8_t> buffer;
    uint32_t count = commitments.size();
    buffer.insert(buffer.end(), (uint8_t*)&count, (uint8_t*)&count + 4);
    for (const auto& commitment : commitments) {
        buffer.insert(buffer.end(), commitment.data(), commitment.data() + 32);
    }
    return utils::to_hex(buffer);
}

std::vector<eddsa_commitment> deserialize_commitments(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    size_t offset = 0;
    uint32_t count;
    memcpy(&count, buffer.data() + offset, 4);
    offset += 4;
    
    std::vector<eddsa_commitment> commitments;
    for (uint32_t i = 0; i < count; i++) {
        eddsa_commitment commitment;
        memcpy(commitment.data(), buffer.data() + offset, 32);
        offset += 32;
        commitments.push_back(commitment);
    }
    return commitments;
}

std::string serialize_eddsa_signatures(const std::vector<eddsa_signature>& sigs) {
    std::vector<uint8_t> buffer;
    uint32_t count = sigs.size();
    buffer.insert(buffer.end(), (uint8_t*)&count, (uint8_t*)&count + 4);
    for (const auto& sig : sigs) {
        buffer.insert(buffer.end(), sig.R, sig.R + 32);
        buffer.insert(buffer.end(), sig.s, sig.s + 32);
    }
    return utils::to_hex(buffer);
}

std::vector<eddsa_signature> deserialize_eddsa_signatures(const std::string& hex) {
    auto buffer = utils::from_hex(hex);
    size_t offset = 0;
    uint32_t count;
    memcpy(&count, buffer.data() + offset, 4);
    offset += 4;
    
    std::vector<eddsa_signature> sigs;
    for (uint32_t i = 0; i < count; i++) {
        eddsa_signature sig;
        memcpy(sig.R, buffer.data() + offset, 32);
        offset += 32;
        memcpy(sig.s, buffer.data() + offset, 32);
        offset += 32;
        sigs.push_back(sig);
    }
    return sigs;
}

} // namespace serialization
} // namespace mpc_demo

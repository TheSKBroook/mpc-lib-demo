#pragma once

#include <string>
#include <vector>
#include "cosigner/asymmetric_eddsa_cosigner_client.h"
#include "cosigner/asymmetric_eddsa_cosigner_server.h"

namespace mpc_demo {
namespace serialization {

/**
 * Serialize EdDSA commitments to hex string
 */
std::string serialize_eddsa_commitments(const std::vector<fireblocks::common::cosigner::eddsa_commitment>& comms);

/**
 * Deserialize EdDSA commitments from hex string
 */
std::vector<fireblocks::common::cosigner::eddsa_commitment> deserialize_eddsa_commitments(const std::string& hex);

/**
 * Serialize elliptic curve points to hex string
 */
std::string serialize_elliptic_curve_points(const std::vector<fireblocks::common::cosigner::elliptic_curve_point>& points);

/**
 * Deserialize elliptic curve points from hex string
 */
std::vector<fireblocks::common::cosigner::elliptic_curve_point> deserialize_elliptic_curve_points(const std::string& hex);

/**
 * Serialize Rs_and_commitments to hex string
 */
std::string serialize_Rs_and_commitments(const fireblocks::common::cosigner::Rs_and_commitments& rs);

/**
 * Deserialize Rs_and_commitments from hex string
 */
fireblocks::common::cosigner::Rs_and_commitments deserialize_Rs_and_commitments(const std::string& hex);

/**
 * Serialize commitments to hex string (alias for eddsa_commitments)
 */
std::string serialize_commitments(const std::vector<fireblocks::common::cosigner::eddsa_commitment>& commitments);

/**
 * Deserialize commitments from hex string (alias for eddsa_commitments)
 */
std::vector<fireblocks::common::cosigner::eddsa_commitment> deserialize_commitments(const std::string& hex);

/**
 * Serialize EdDSA signatures to hex string
 */
std::string serialize_eddsa_signatures(const std::vector<fireblocks::common::cosigner::eddsa_signature>& sigs);

/**
 * Deserialize EdDSA signatures from hex string
 */
std::vector<fireblocks::common::cosigner::eddsa_signature> deserialize_eddsa_signatures(const std::string& hex);

} // namespace serialization
} // namespace mpc_demo

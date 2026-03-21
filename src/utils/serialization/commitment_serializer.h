#pragma once

#include <string>
#include "cosigner/cmp_setup_service.h"

namespace mpc_demo {
namespace serialization {

/**
 * Serialize commitment to hex string
 */
std::string serialize_commitment(const fireblocks::common::cosigner::commitment& comm);

/**
 * Deserialize commitment from hex string
 */
fireblocks::common::cosigner::commitment deserialize_commitment(const std::string& hex);

/**
 * Serialize decommitment to hex string
 */
std::string serialize_decommitment(const fireblocks::common::cosigner::setup_decommitment& decomm);

/**
 * Deserialize decommitment from hex string
 */
fireblocks::common::cosigner::setup_decommitment deserialize_decommitment(const std::string& hex);

} // namespace serialization
} // namespace mpc_demo

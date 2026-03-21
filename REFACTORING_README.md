# MPC Demo - Refactored Code Structure

## Overview

This codebase has been refactored from a single monolithic file (`mpc_demo_network.cpp`) into a well-organized, modular structure. The logic and functionality remain unchanged - only the organization has been improved.

## Directory Structure

```
src/
├── main.cpp                                  # Entry point (minimal, delegates to orchestrators)
│
├── config/
│   └── mpc_config.h                          # Configuration constants
│
├── utils/
│   ├── hex_utils.h/cpp                       # Hex serialization utilities
│   └── serialization/
│       ├── commitment_serializer.h/cpp       # Commitment/decommitment serialization
│       └── eddsa_serializer.h/cpp            # EdDSA types serialization
│
├── network/
│   ├── network_manager.h                     # Network communication layer
│   └── network_manager.cpp
│
├── persistence/
│   ├── in_memory_key_persistency.h/cpp       # Key storage implementation
│   ├── mock_client_persistency.h/cpp         # Client preprocessing storage
│   └── mock_server_persistency.h/cpp         # Server signing storage
│
├── platform/
│   ├── simple_platform.h                     # Platform service implementation
│   └── simple_platform.cpp
│
├── mpc/
│   ├── keygen_orchestrator.h/cpp             # Key generation workflow
│   ├── preprocessing_orchestrator.h/cpp      # Preprocessing workflow
│   └── signing_orchestrator.h/cpp            # Signing workflow
│
└── CMakeLists_refactored.txt                 # Updated build configuration
```

## Module Descriptions

### 1. Configuration (`config/`)
- **Purpose**: Single source of truth for all configuration constants
- **Files**: `mpc_config.h`
- **Contents**: RELAY_URL, TENANT_ID, KEY_ID, TOTAL_PLAYERS, MPC_PROTOCOL_VERSION

### 2. Utilities (`utils/`)
- **Purpose**: Reusable helper functions and serialization logic
- **Hex Utils**: Conversion between bytes and hexadecimal strings
- **Serialization**: 
  - Commitment/decommitment structures
  - EdDSA types (commitments, points, signatures, Rs_and_commitments)

### 3. Network (`network/`)
- **Purpose**: Handle all HTTP communication with relay server
- **Key Functions**:
  - `broadcast()`: Send data to all nodes
  - `await_data()`: Wait for data from all nodes
  - `await_data_custom_count()`: Wait for specific number of responses

### 4. Persistence (`persistence/`)
- **Purpose**: Data storage implementations for MPC components
- **Classes**:
  - `InMemoryKeyPersistency`: Stores MPC key shares and metadata
  - `MockClientPersistency`: Stores client preprocessing data
  - `MockServerPersistency`: Stores server signing data

### 5. Platform (`platform/`)
- **Purpose**: Platform-specific services (random generation, encryption, etc.)
- **Class**: `SimplePlatform` - implements required platform interface

### 6. MPC Orchestrators (`mpc/`)
- **Purpose**: High-level workflow coordination for MPC protocols
- **Classes**:
  - `KeygenOrchestrator`: Manages distributed key generation
  - `PreprocessingOrchestrator`: Manages nonce preprocessing
  - `SigningOrchestrator`: Manages distributed signing (client/server flows)

### 7. Main (`main.cpp`)
- **Purpose**: Minimal entry point
- **Responsibilities**:
  - Parse command-line arguments
  - Initialize components
  - Run interactive menu loop
  - Delegate to orchestrators

## Key Benefits

✅ **Modularity**: Each component has a single, clear responsibility  
✅ **Testability**: Individual modules can be unit tested in isolation  
✅ **Maintainability**: Easy to locate and modify specific functionality  
✅ **Reusability**: Components can be reused in other projects  
✅ **Readability**: Smaller, focused files are easier to understand  
✅ **Scalability**: Simple to add new features or protocols  
✅ **Namespace Organization**: All code organized under `mpc_demo` namespace

## Building the Refactored Code

```bash
# Copy the new CMakeLists
cp src/CMakeLists_refactored.txt CMakeLists.txt

# Create build directory
mkdir -p build && cd build

# Configure and build
cmake ..
make

# Run (same as before)
./mpc_app 1  # Node 1
./mpc_app 2  # Node 2
./mpc_app 3  # Node 3
```

## Original vs Refactored

| Aspect | Original | Refactored |
|--------|----------|------------|
| **Files** | 1 monolithic file (900+ lines) | 26 focused files |
| **Organization** | Everything mixed together | Clear module separation |
| **Testing** | Difficult to test components | Easy to unit test |
| **Reusability** | Hard to reuse code | Components are reusable |
| **Navigation** | Search through 1 big file | Jump directly to module |
| **Collaboration** | Merge conflicts likely | Multiple devs can work simultaneously |

## Code Preservation

**Important**: The refactoring preserves 100% of the original logic:
- All MPC protocol flows are identical
- Network communication unchanged
- Serialization/deserialization logic preserved
- Error handling maintained
- Comments and logic preserved where meaningful

## Next Steps

Potential improvements (not included in this refactoring):
- Add unit tests for each module
- Implement proper error handling strategies
- Add logging framework
- Make configuration file-based instead of hardcoded
- Add protocol validation
- Implement proper encryption in platform service

## Migration Notes

If you need to switch back to the original monolithic version:
- Original code preserved in: `mpc_demo_network.cpp`
- Use original CMakeLists.txt for building

The refactored version is ready to use and maintains full compatibility with the relay server and MPC protocol.

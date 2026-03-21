# Refactored MPC Demo Architecture

## Component Dependency Graph

```
┌─────────────────────────────────────────────────────────────────┐
│                           main.cpp                               │
│                      (Entry Point)                               │
└───────────────────────┬─────────────────────────────────────────┘
                        │
        ┌───────────────┼───────────────┐
        │               │               │
        ▼               ▼               ▼
┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│   Keygen     │ │Preprocessing │ │   Signing    │
│ Orchestrator │ │ Orchestrator │ │ Orchestrator │
└──────┬───────┘ └──────┬───────┘ └──────┬───────┘
       │                │                │
       │    ┌───────────┴────────────┐   │
       │    │                        │   │
       ▼    ▼                        ▼   ▼
   ┌─────────────────────────────────────────┐
   │         Network Manager                  │
   │  ┌─────────────────────────────────┐    │
   │  │ • broadcast()                    │    │
   │  │ • await_data()                   │    │
   │  │ • await_data_custom_count()      │    │
   │  └─────────────────────────────────┘    │
   └─────────────────────────────────────────┘
       │                        │
       ▼                        ▼
┌──────────────┐         ┌──────────────┐
│ Serializers  │         │ Hex Utils    │
├──────────────┤         └──────────────┘
│ • Commitment │
│ • EdDSA      │
└──────────────┘

   ┌──────────────────────────────────────────┐
   │         Persistence Layer                 │
   ├──────────────────────────────────────────┤
   │  InMemoryKeyPersistency                   │
   │  MockClientPersistency                    │
   │  MockServerPersistency                    │
   └──────────────────────────────────────────┘

   ┌──────────────────────────────────────────┐
   │         Platform Service                  │
   ├──────────────────────────────────────────┤
   │  SimplePlatform                           │
   │  • Random generation                      │
   │  • Tenant/ID management                   │
   │  • Client identification                  │
   └──────────────────────────────────────────┘

   ┌──────────────────────────────────────────┐
   │         Configuration                     │
   ├──────────────────────────────────────────┤
   │  mpc_config.h (constants)                 │
   └──────────────────────────────────────────┘
```

## Data Flow: Key Generation

```
main.cpp
   │
   ├─> Create KeygenOrchestrator
   │
   └─> KeygenOrchestrator::run()
          │
          ├─> cmp_setup_service.generate_setup_commitments()
          │      └─> InMemoryKeyPersistency (store setup data)
          │
          ├─> serialize_commitment() → NetworkManager.broadcast()
          │
          ├─> NetworkManager.await_data() → deserialize_commitment()
          │
          ├─> cmp_setup_service.store_setup_commitments()
          │
          ├─> serialize_decommitment() → NetworkManager.broadcast()
          │
          ├─> NetworkManager.await_data() → deserialize_decommitment()
          │
          └─> Compute public key → store metadata
```

## Data Flow: Preprocessing

```
main.cpp
   │
   ├─> Create PreprocessingOrchestrator
   │
   └─> PreprocessingOrchestrator::run()
          │
          ├─ CLIENT:
          │    └─> asymmetric_eddsa_cosigner_client.start_signature_preprocessing()
          │           └─> MockClientPersistency (store nonces)
          │           └─> NetworkManager.broadcast(R_commitments)
          │
          └─ SERVER:
               └─> NetworkManager.await_data(client R_commitments)
               └─> asymmetric_eddsa_cosigner_server.store_presigning_data()
                      └─> MockServerPersistency (store commitments)
```

## Data Flow: Signing

```
main.cpp
   │
   ├─> Create SigningOrchestrator
   │
   └─> SigningOrchestrator::run()
          │
          ├─ Synchronization barrier (all nodes ready)
          │
          ├─ CLIENT FLOW:
          │    ├─> await server R-commitments
          │    ├─> await server Rs
          │    ├─> eddsa_sign_offline() → generate partial sigs
          │    ├─> broadcast partial sigs
          │    └─> receive final signature
          │
          └─ SERVER FLOW:
               ├─> generate server R-commitments
               ├─> broadcast commitments
               ├─> decommit_r()
               ├─> exchange Rs with other servers
               ├─> broadcast_r() → aggregate
               ├─> send Rs to client
               ├─> await client partial sigs
               ├─> broadcast_si() → process
               └─> get_eddsa_signature() → final sig
```

## Module Interaction Matrix

|                    | Config | Utils | Network | Persist | Platform | MPC Orch |
|--------------------|--------|-------|---------|---------|----------|----------|
| **main.cpp**       |   ✓    |       |    ✓    |    ✓    |    ✓     |    ✓     |
| **MPC Orchestrators** | ✓   |   ✓   |    ✓    |    ✓    |    ✓     |          |
| **Network Manager**|   ✓    |   ✓   |         |         |          |          |
| **Serializers**    |        |   ✓   |         |         |          |          |
| **Persistence**    |   ✓    |       |         |         |          |          |
| **Platform**       |   ✓    |       |         |         |          |          |

✓ = Uses/Depends on

## Namespace Organization

```
namespace mpc_demo {
    namespace config {
        // Configuration constants
    }
    
    namespace utils {
        // Hex utilities
    }
    
    namespace serialization {
        // Serialization functions
    }
    
    namespace network {
        class NetworkManager { ... }
    }
    
    namespace persistence {
        class InMemoryKeyPersistency { ... }
        class MockClientPersistency { ... }
        class MockServerPersistency { ... }
    }
    
    namespace platform {
        class SimplePlatform { ... }
    }
    
    namespace mpc {
        class KeygenOrchestrator { ... }
        class PreprocessingOrchestrator { ... }
        class SigningOrchestrator { ... }
    }
}
```

## File Size Comparison

| Original File | Size | Refactored Files | Avg Size |
|---------------|------|------------------|----------|
| mpc_demo_network.cpp | 900+ lines | 26 files | ~50-150 lines each |

## Build Dependencies

```
CMakeLists.txt
   │
   ├─> Collect all .cpp files from:
   │     • utils/
   │     • network/
   │     • persistence/
   │     • platform/
   │     • mpc/
   │     • main.cpp
   │
   ├─> Link libraries:
   │     • CURL
   │     • MPC-lib components
   │     • GMP, pthread
   │
   └─> Create executable: mpc_app
```

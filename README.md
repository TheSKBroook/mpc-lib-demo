# MPC Threshold Signing Demo

A hands-on, three-node demonstration of **threshold EdDSA signing** built on top of
[Fireblocks' MPC library](https://github.com/fireblocks/mpc-lib) (`libcosigner`).

Three independent processes jointly generate an Ed25519 key that **no single process ever holds**,
and then cooperate to produce a valid 64-byte signature over a message. Each node keeps only its
own secret share; the private key is never assembled anywhere, at any point.

```
                         ┌──────────────────────┐
                         │  Relay Server        │
                         │  (Flask, port 5000)  │
                         │  store & forward     │
                         └───┬──────┬───────┬───┘
                             │      │       │
              ┌──────────────┘      │       └──────────────┐
              │                     │                      │
      ┌───────▼───────┐    ┌────────▼──────┐    ┌──────────▼────┐
      │   Node 1      │    │    Node 2     │    │    Node 3     │
      │   CLIENT      │    │    SERVER     │    │    SERVER     │
      │  share_1      │    │   share_2     │    │   share_3     │
      └───────────────┘    └───────────────┘    └───────────────┘

        Combined public key = share_1·G + share_2·G + share_3·G
        Private key         = never exists in one place
```

---

## What This Does

Each node is a separate OS process (`mpc_app <node_id>`) that walks through the full lifecycle of an
asymmetric-EdDSA MPC signature:

| Phase | What happens | Who drives it |
|-------|--------------|---------------|
| **1. Key Generation** | Commit–reveal setup rounds via `cmp_setup_service`. Every node produces a secret share locally; the group public key is aggregated from the public shares. | All 3 nodes |
| **2. Preprocessing** | The client pre-generates a batch of 1000 nonce commitments (`R`-commitments) and ships them to the servers, which store them for later use. | Client generates, servers store |
| **3. Signing** | Servers commit to their own `R` values, decommit, aggregate, and send the combined `R` set to the client. The client produces partial signatures; the servers combine them into the final signature. | Client + both servers |

The signature is printed as `R || s` (64 bytes) — a standard Ed25519 signature that verifies against
the aggregated public key.

## What This Demonstrates

- **Distributed key generation** — a shared Ed25519 public key derived from independently generated
  secret shares, with the private key never materialised.
- **The asymmetric EdDSA protocol** — the real client/server split from `libcosigner`, where a
  lightweight client can go offline between preprocessing and signing.
- **Nonce preprocessing** — why signing is fast: the expensive nonce generation is batched up front,
  decoupled from the moment you actually want to sign.
- **Commit-reveal discipline** — nodes commit to their `R` values before revealing them, so no
  participant can bias the nonce after seeing the others'.
- **Multi-round network orchestration** — how to drive an interactive MPC protocol over a real
  transport, including per-round synchronization barriers and message serialization.
- **Clean integration boundaries** — the four interfaces `libcosigner` expects you to supply
  (`platform_service`, key persistency, client persistency, server persistency) implemented as
  small, readable classes.

---

## Requirements

The MPC library targets Linux. **Ubuntu 20.04 / 22.04 / 24.04** (or WSL2 on Windows) is the
supported path.

| Requirement | Notes |
|-------------|-------|
| C++17 compiler | GCC 9+ / Clang 10+ |
| CMake | ≥ 3.14 |
| OpenSSL | ≥ 1.1.1 (`libssl-dev`) |
| libuuid | `uuid-dev` |
| libsecp256k1 | `libsecp256k1-dev` |
| libcurl | `libcurl4-openssl-dev` — used by the node's network layer |
| Python | 3.8+ with Flask — runs the message relay |

Install everything in one go:

```bash
sudo apt update && sudo apt install -y build-essential cmake libssl-dev uuid-dev libsecp256k1-dev libcurl4-openssl-dev python3 python3-pip && pip3 install flask
```

---

## Install and Run

### 1. Build

```bash
mkdir -p build && cd build && cmake .. && make -j$(nproc)
```

This produces two binaries in `build/`:

- **`mpc_app`** — the modular version (`src/main.cpp` + orchestrators). Use this one.
- **`mpc_node`** — the original single-file version, kept for reference.

### 2. Start the relay server

The relay is a dumb store-and-forward broker: nodes POST messages tagged with a round ID, and poll
until every expected participant has posted for that round. It performs no cryptography.

```bash
python3 src/relay_server.py
```

Leave it running on `http://127.0.0.1:5000`.

### 3. Start the three nodes

Open three more terminals, one per node. **Node 1 is the client; nodes 2 and 3 are servers.**

```bash
./build/mpc_app 1
```

```bash
./build/mpc_app 2
```

```bash
./build/mpc_app 3
```

Key generation begins automatically as soon as all three are up.

### 4. Run a signing session

Once keygen completes, each node shows a menu:

```
[1] Start New Signing Session
[2] Join Signing Session
>
```

1. On **node 1**, choose `1`. It prints a session ID such as `tx-1734...`.
2. On **nodes 2 and 3**, choose `2` and paste that same session ID.
3. Preprocessing runs on all three nodes.
4. Each node then prompts `>>> Type 'start' and press Enter to begin signing:` — type `start` on all
   three.

Node 1 prints the result:

```
=== SIGNATURE SUCCESS ===
Message: "Hello from distributed MPC!"
Full Signature R (32 bytes): a3f2...
Full Signature s (32 bytes): 7c19...
Combined Signature (64 bytes): a3f2...7c19...
=========================
```

> **Tip:** to run a fresh session from scratch, restart the relay (or `curl -X POST
> http://127.0.0.1:5000/reset`) before restarting the nodes, so stale round data doesn't leak into
> the new run.

---

## Configuration

All knobs live in [`src/config/mpc_config.h`](src/config/mpc_config.h):

| Constant | Default | Meaning |
|----------|---------|---------|
| `RELAY_URL` | `http://127.0.0.1:5000` | Where nodes broadcast and poll |
| `TENANT_ID` | `DEMO_TENANT` | Logical tenant namespace |
| `KEY_ID` | `demo-key-1` | Identifier for the generated key |
| `TOTAL_PLAYERS` | `3` | Number of participating nodes |
| `PREPROCESSING_COUNT` | `1000` | Nonces pre-generated per preprocessing run |

The message being signed is hardcoded in
[`src/mpc/signing_orchestrator.cpp`](src/mpc/signing_orchestrator.cpp) (`"Hello from distributed
MPC!"`). Changing `TOTAL_PLAYERS` also requires updating the hardcoded player sets in the
orchestrators.

---

## Project Layout

```
.
├── mpc-lib/                      # Fireblocks MPC library (libcosigner)
├── src/
│   ├── main.cpp                  # Entry point — wires components, runs the menu loop
│   ├── config/mpc_config.h       # All configuration constants
│   ├── mpc/                      # Protocol orchestrators
│   │   ├── keygen_orchestrator       # Distributed key generation
│   │   ├── preprocessing_orchestrator# Nonce batch generation & distribution
│   │   └── signing_orchestrator      # Client and server signing flows
│   ├── network/network_manager   # HTTP broadcast / poll over libcurl
│   ├── persistence/              # Key, client, and server storage (in-memory)
│   ├── platform/simple_platform  # platform_service implementation
│   ├── utils/                    # Hex helpers + wire serialization
│   ├── relay_server.py           # Flask message broker
│   └── mpc_demo_network.cpp      # Original monolithic version (reference)
└── CMakeLists.txt
```

For a deeper walkthrough of module responsibilities and per-phase message flows, see
[ARCHITECTURE.md](ARCHITECTURE.md).

---

## Demo Scope and Limitations

This is a **learning and integration demo, not production code**. Specifically:

- **Keygen is partially mocked.** The commit and reveal rounds are real, but the zero-knowledge
  proof and Paillier rounds are skipped; key metadata is assembled directly instead
  (`keygen_orchestrator.cpp`). A production deployment must run the full setup protocol.
- **No transport security.** Messages travel to the relay as plaintext hex over HTTP, and
  `SimplePlatform::encrypt_for_player` / `decrypt_message` are pass-through no-ops.
- **Non-cryptographic RNG.** `SimplePlatform::gen_random` uses `std::mt19937`, not a CSPRNG.
- **In-memory persistence only.** All shares and preprocessing state vanish when a node exits — the
  key cannot survive a restart.
- **Centralized, trusted relay.** A single broker sees every message and is a single point of
  failure. It can't forge signatures, but it can stall the protocol.
- **Fixed roles and topology.** Exactly three nodes, with node 1 hardcoded as the client.

## License

The bundled MPC library is distributed under the GNU General Public License — see
[mpc-lib/LICENSE](mpc-lib/LICENSE).

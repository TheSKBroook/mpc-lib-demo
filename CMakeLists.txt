cmake_minimum_required(VERSION 3.14)
project(MPCDemoNetwork)

# Set C++ Standard to 17 (Required for modern features used)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Add mpc-lib subdirectory ---
add_subdirectory(mpc-lib)

# --- Dependencies ---

# 1. Find libcurl (Required for NetworkManager)
find_package(CURL REQUIRED)

# 2. Find Threads (Required for std::this_thread::sleep_for)
find_package(Threads REQUIRED)

# --- Include Directories ---
include_directories(${CMAKE_SOURCE_DIR}/src)

# --- Refactored MPC Demo Executable ---

# Collect all source files for the refactored structure
set(SOURCES
    # Main entry point
    src/main.cpp
    
    # Utilities
    src/utils/hex_utils.cpp
    src/utils/serialization/commitment_serializer.cpp
    src/utils/serialization/eddsa_serializer.cpp
    
    # Network
    src/network/network_manager.cpp
    
    # Persistence
    src/persistence/in_memory_key_persistency.cpp
    src/persistence/mock_client_persistency.cpp
    src/persistence/mock_server_persistency.cpp
    
    # Platform
    src/platform/simple_platform.cpp
    
    # MPC Orchestrators
    src/mpc/keygen_orchestrator.cpp
    src/mpc/preprocessing_orchestrator.cpp
    src/mpc/signing_orchestrator.cpp
)

# Create refactored executable
add_executable(mpc_app ${SOURCES})

# Link libraries
target_link_libraries(mpc_app PRIVATE 
    cosigner
    CURL::libcurl 
    Threads::Threads
)

# --- Old Monolithic Version (currently active until refactoring compiles) ---
add_executable(mpc_node src/mpc_demo_network.cpp)
target_link_libraries(mpc_node PRIVATE 
    cosigner
    CURL::libcurl 
    Threads::Threads
)
cmake_minimum_required(VERSION 3.20)
project(options_replay_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(engine
  src/main.cpp

  src/feed/csv.cpp
  src/feed/csv_reader.cpp

  src/market/market_state.cpp
  src/pricing/black_scholes.cpp

  src/strategy/edge_threshold.cpp
)

target_include_directories(engine PRIVATE include)

# Warnings (good defaults)
target_compile_options(engine PRIVATE -Wall -Wextra -Wpedantic)

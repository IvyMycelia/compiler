cmake_minimum_required(VERSION 3.20)
set(CMAKE_C_STANDARD 11)
project(Flower C)


# === SOURCES ===
file(GLOB s "src/*.c")
add_executable(Flower ${s})


# === Include Headers ===
target_include_directories(Flower PRIVATE "include")


# === THIRD PARTIES ===
# Not Applicable ¯\_(ツ)_/¯


# === OUTPUT BINARY ===
set_target_properties(Flower PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
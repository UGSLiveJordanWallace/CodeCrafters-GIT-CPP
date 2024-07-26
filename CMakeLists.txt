cmake_minimum_required(VERSION 3.13)

project(git-starter-cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

add_executable(git ${SOURCE_FILES})
target_link_libraries(git PRIVATE ZLIB::ZLIB OpenSSL::SSL OpenSSL::Crypto)

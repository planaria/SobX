cmake_minimum_required(VERSION 3.8.0)
project(sobx LANGUAGES CXX VERSION 0.1.0)

add_library(sobx INTERFACE)
target_include_directories(sobx INTERFACE include)

enable_testing()
add_subdirectory(tests)

install(DIRECTORY include DESTINATION ".")

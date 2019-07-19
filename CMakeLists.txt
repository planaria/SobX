cmake_minimum_required(VERSION 3.0.0)
project(sobx LANGUAGES CXX VERSION 0.1.0)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang|GNU|Intel")
    set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS "/std:c++17 ${CMAKE_CXX_FLAGS}")
endif()

add_library(sobx INTERFACE)
target_include_directories(sobx INTERFACE include)

add_subdirectory(tests)

install(DIRECTORY include DESTINATION ".")

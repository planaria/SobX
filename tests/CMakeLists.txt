find_package(Catch2 REQUIRED)

file(GLOB SRC "*.cpp")

add_executable(sobx_test ${SRC})
include_directories(../include)
target_link_libraries(sobx_test Catch2::Catch2)

enable_testing()
add_test(NAME sobx COMMAND sobx_test)

include_directories(${CATCH_INCLUDE_DIR})

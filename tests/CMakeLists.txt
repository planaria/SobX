find_package(Catch2 REQUIRED)

file(GLOB SRC "*.cpp")

add_executable(sobx_test ${SRC})

set_property(TARGET sobx_test PROPERTY CXX_STANDARD 17)
set_property(TARGET sobx_test PROPERTY CXX_STANDARD_REQUIRED ON)

include_directories(../include)
target_link_libraries(sobx_test Catch2::Catch2)

add_test(NAME sobx COMMAND sobx_test)

include_directories(${CATCH_INCLUDE_DIR})

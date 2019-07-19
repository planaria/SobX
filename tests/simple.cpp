#include <sobx/sobx.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("simple")
{
    sobx::observable<int> x = 1;
    std::vector<int> values;

    auto sub = sobx::autorun([&]() {
        values.push_back(x);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = 2;
    });

    CHECK(values == std::vector<int>{1, 2});

    sobx::run_in_action([&]() {
        x = 3;
    });

    CHECK(values == std::vector<int>{1, 2, 3});
}

#include <sobx/sobx.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("multiple")
{
    sobx::observable<int> x = 1;
    sobx::observable<int> y = 1;
    std::vector<int> values;

    sobx::autorun([&]() {
        values.push_back(x * y);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = 2;
    });

    CHECK(values == std::vector<int>{1, 2});

    sobx::run_in_action([&]() {
        y = 3;
    });

    CHECK(values == std::vector<int>{1, 2, 6});

    sobx::run_in_action([&]() {
        x = 5;
        y = 7;
    });

    CHECK(values == std::vector<int>{1, 2, 6, 35});
}

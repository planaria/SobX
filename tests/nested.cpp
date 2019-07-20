#include <sobx/sobx.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("nested_run_in_action")
{
    sobx::observable<int> x = 1;
    std::vector<int> values;

    auto sub = sobx::autorun([&]() {
        values.push_back(x);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = 2;

        sobx::run_in_action([&]() {
            x = 3;
        });

        x = 4;
    });

    CHECK(values == std::vector<int>{1, 4});
}

TEST_CASE("nested_autorun_1")
{
    sobx::observable<int> x, y, z;

    auto sub1 = sobx::autorun([&]() {
        z = y;

        auto sub2 = sobx::autorun([&]() {
            y = x;
        });
    });

    sobx::run_in_action([&]() {
        x = 123;
    });

    CHECK(x == 123);
}

TEST_CASE("nested_autorun_2")
{
    sobx::observable<int> x, y, z;

    auto sub1 = sobx::autorun([&]() {
        auto sub2 = sobx::autorun([&]() {
            y = x;
        });

        z = y;
    });

    sobx::run_in_action([&]() {
        x = 123;
    });

    CHECK(x == 123);
}

#include <sobx/sobx.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("switching")
{
    sobx::observable<bool> flag = true;
    sobx::observable<int> x = 1;
    sobx::observable<int> y = 100;
    std::vector<int> values;

    auto disposer = sobx::autorun([&]() {
        values.push_back(flag.get() ? x : y);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = 2;
    });

    CHECK(values == std::vector<int>{1, 2});

    sobx::run_in_action([&]() {
        flag = false;
    });

    CHECK(values == std::vector<int>{1, 2, 100});

    sobx::run_in_action([&]() {
        x = 3;
    });

    CHECK(values == std::vector<int>{1, 2, 100});

    sobx::run_in_action([&]() {
        y = 101;
    });

    CHECK(values == std::vector<int>{1, 2, 100, 101});

    sobx::run_in_action([&]() {
        flag = true;
    });

    CHECK(values == std::vector<int>{1, 2, 100, 101, 3});
}

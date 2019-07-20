#include <sobx/sobx.hpp>
#include <catch.hpp>
#include <vector>

TEST_CASE("dispose_1")
{
    sobx::observable<int> x = 1;
    std::vector<int> values;

    auto disposer = sobx::autorun([&](auto &reaction) {
        values.push_back(x);
        x = x * 2;

        if (x >= 100)
        {
            reaction.dispose();
        }
    });

    CHECK(values == std::vector<int>{1, 2, 4, 8, 16, 32, 64});
}

TEST_CASE("dispose_2")
{
    sobx::observable<int> x = 1;
    std::vector<int> values;

    auto disposer = sobx::autorun([&](auto &reaction) {
        values.push_back(x);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = 2;
    });

    CHECK(values == std::vector<int>{1, 2});

    disposer.dispose();

    sobx::run_in_action([&]() {
        x = 3;
    });

    CHECK(values == std::vector<int>{1, 2});
}

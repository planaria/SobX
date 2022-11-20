#include <sobx/sobx.hpp>
#include <catch2/catch_all.hpp>
#include <vector>

TEST_CASE("same_value")
{
    sobx::observable<int> x;
    std::vector<int> values;

    auto disposer = sobx::autorun([&]() {
        values.push_back(x);
    });

    sobx::run_in_action([&]() {
        x = 123;
    });

    CHECK(values == std::vector<int>{0, 123});

    sobx::run_in_action([&]() {
        x = 123;
    });

    CHECK(values == std::vector<int>{0, 123});
}

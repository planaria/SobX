#include <sobx/sobx.hpp>
#include <catch2/catch_all.hpp>
#include <vector>

TEST_CASE("recursive")
{
    sobx::observable<int> x = 1;
    std::vector<int> values;

    auto disposer = sobx::autorun([&]() {
        if (x < 100)
        {
            values.push_back(x);
            x = x * 2;
        }
    });

    CHECK(values == std::vector<int>{1, 2, 4, 8, 16, 32, 64});
}

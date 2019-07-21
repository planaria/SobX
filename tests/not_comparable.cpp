#include <sobx/sobx.hpp>
#include <catch.hpp>
#include <vector>
#include <optional>

namespace
{

struct test
{
    test(int value)
        : value(value)
    {
    }

    int value = 0;
};

} // namespace

TEST_CASE("not_comparable")
{
    sobx::observable<test> x = 1;
    std::vector<int> values;

    auto disposer = sobx::autorun([&]() {
        values.push_back(x.get().value);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = test(2);
    });

    CHECK(values == std::vector<int>{1, 2});

    sobx::run_in_action([&]() {
        x = test(2);
    });

    CHECK(values == std::vector<int>{1, 2, 2});
}

TEST_CASE("not_comparable_optional")
{
    sobx::observable<std::optional<test>> x = 1;
    std::vector<int> values;

    auto disposer = sobx::autorun([&]() {
        values.push_back(x.get()->value);
    });

    CHECK(values == std::vector<int>{1});

    sobx::run_in_action([&]() {
        x = test(2);
    });

    CHECK(values == std::vector<int>{1, 2});

    sobx::run_in_action([&]() {
        x = test(2);
    });

    CHECK(values == std::vector<int>{1, 2, 2});
}

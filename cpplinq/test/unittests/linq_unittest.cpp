#include "gtest/gtest.h"

#define USE_CPPLINQ_MACRO
#include "cpplinq.h"

struct Table
{
    int x;
    int y;
};

TEST(CppLinq, basic)
{
    std::vector<Table> numbers = { {1, 10}, {2, 12}, {3, 11}, {4, 13} };

    auto result = CPPLINQ(Table)
        FROM (numbers)
        WHERE (o.x % 2 == 0)
        ORDERBY (o.x)
        SELECT (int, int) ON (o.x, o.y * o.y);

    std::vector<std::tuple<int,int>> expectedResult = { { 4, 169 }, { 2, 144 } };
    EXPECT_EQ(result, expectedResult); 
}

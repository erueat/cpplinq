#include "gtest/gtest.h"

#define USE_LINQ_MACRO
#include "linq.h"

using namespace zen;
using std::vector;
using std::tuple;

struct Table
{
    int x;
    int y;
};

TEST(Linq, basic)
{
    vector<Table> numbers = { {1, 10}, {2, 12}, {3, 11}, {4, 13} };

    auto result = LINQ(Table)
        FROM (numbers)
        WHERE (o.x % 2 == 0)
        ORDERBY (o.x)
        SELECT (int, int) ON (o.x, o.y * o.y);

    vector<tuple<int,int>> expectedResult = { { 4, 169 }, { 2, 144 } };
    EXPECT_EQ(result, expectedResult); 
}

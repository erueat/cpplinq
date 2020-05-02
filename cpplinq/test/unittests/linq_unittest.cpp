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
        SELECT (int, int) ON (o.x, o.y * o.y);

    std::vector<std::tuple<int,int>> expectedResult = { { 2, 144 }, { 4, 169 } };
    EXPECT_EQ(result, expectedResult); 
}

TEST(CppLinq, take)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto result = CPPLINQ(int)
        FROM (numbers)
        WHERE (o % 2 == 0)
        TAKE (3)
        SELECT (int) ON (o);

    std::vector<std::tuple<int>> expectedResult = { { 2 }, { 4 }, { 6 } };
    EXPECT_EQ(result, expectedResult);
}

TEST(CppLinq, skip)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto result = CPPLINQ(int)
        FROM (numbers)
        WHERE (o % 2 == 0)
        SKIP (3)
        SELECT (int) ON (o);

    std::vector<std::tuple<int>> expectedResult = { { 8 }, { 10 } };
    EXPECT_EQ(result, expectedResult);
}

TEST(CppLinq, orderBy)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    
    auto result = CPPLINQ(int)
        FROM (numbers)
        WHERE (o % 2 == 0)
        ORDERBY (o)
        SELECT (int) ON (o);

    std::vector<std::tuple<int>> expectedResult = { { 2 }, { 4 }, { 6 }, { 8 }, { 10 } };
    EXPECT_EQ(result, expectedResult);

    auto result2 = CPPLINQ(int)
        FROM (numbers)
        WHERE (o % 2 == 0)
        ORDERBY (o, DESCEND)
        SELECT (int) ON (o);

    std::vector<std::tuple<int>> expectedResult2 = { { 10 }, { 8 }, { 6 }, { 4 }, { 2 } };
    EXPECT_EQ(result2, expectedResult2);
}

TEST(CppLinq, firstAndLast)
{
    std::vector<int> numbers = { 1, 2, 3 };

    auto result = CPPLINQ(int)
        FROM (numbers)
        FIRST;
    
    EXPECT_EQ(result, 1);

    auto result2 = CPPLINQ(int)
        FROM (numbers)
        LAST;
    
    EXPECT_EQ(result2, 3);
}
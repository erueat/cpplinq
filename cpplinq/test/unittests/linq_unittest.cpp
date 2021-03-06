#include "gtest/gtest.h"

#define USE_CPPLINQ_MACRO
#include "cpplinq.h"

#include <list>

TEST(CppLinq, basic)
{
    struct Record
    {
        int x;
        int y;
    };

    std::vector<Record> records = { { 1, 10 }, { 2, 12 }, { 3, 11 }, { 4, 13 } };

    auto result = FROM (records)
        WHERE (o.x % 2 == 0)
        SELECT (o.x, o.y * o.y);

    std::vector<std::tuple<int,int>> expectedResult = { { 2, 144 }, { 4, 169 } };
    EXPECT_EQ(result, expectedResult); 
}

TEST(CppLinq, take)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        TAKE (3)
        SELECT (o);

    std::vector<std::tuple<int>> expectedResult = { { 2 }, { 4 }, { 6 } };
    EXPECT_EQ(result, expectedResult);
}

TEST(CppLinq, skip)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        SKIP (3)
        SELECT (o);

    std::vector<std::tuple<int>> expectedResult = { { 8 }, { 10 } };
    EXPECT_EQ(result, expectedResult);
}

TEST(CppLinq, orderBy)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    
    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        ORDERBY (o)
        SELECT (o);

    std::vector<std::tuple<int>> expectedResult = { { 2 }, { 4 }, { 6 }, { 8 }, { 10 } };
    EXPECT_EQ(result, expectedResult);

    auto result2 = FROM (numbers)
        WHERE (o % 2 == 0)
        ORDERBY (o, DESCEND)
        SELECT (o);

    std::vector<std::tuple<int>> expectedResult2 = { { 10 }, { 8 }, { 6 }, { 4 }, { 2 } };
    EXPECT_EQ(result2, expectedResult2);
}

TEST(CppLinq, firstAndLast)
{
    std::vector<int> numbers = { 1, 2, 3 };

    auto result = FROM (numbers)
        FIRST();
    
    EXPECT_EQ(result, 1);

    auto result2 = FROM (numbers)
        LAST();
    
    EXPECT_EQ(result2, 3);
}

TEST(CppLinq, count)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };

    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        COUNT();
    
    EXPECT_EQ(result, 3);
}

TEST(CppLinq, sum)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };

    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        SUM();
    
    EXPECT_EQ(result, 12);
}

TEST(CppLinq, average)
{
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };

    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        AVERAGE();
    
    EXPECT_EQ(result, 4);
}

TEST(CppLinq, supportCArray)
{
    int numbers[] = { 1, 2, 3, 4 };

    auto result = FROM (numbers)
        WHERE (o % 2 == 0)
        SELECT (o);

    std::vector<std::tuple<int>> expectedResult = { 2, 4 };
    EXPECT_EQ(result, expectedResult);
}

TEST(CppLinq, supportList)
{
    std::list<int> numbers = { 1, 2, 3, 4 };

    auto result = FROM (numbers) 
        WHERE (o % 2 == 0) 
        SELECT (o);

    std::vector<std::tuple<int>> expectedResult = { 2, 4 };
    EXPECT_EQ(result, expectedResult);
}

TEST(CppLinq, join)
{
    struct Record1
    {
        int x;
        int y;
    };

    struct Record2
    {
        int a;
        int b;
    };

    struct Record3
    {
        int m;
        int n;
    };

    Record1 records1[] = { { 1, 100 }, { 2, 200 }, { 1, 300 }, { 5, 341 }, { 5, 400 }, { 5, 965 } };
    Record2 records2[] = { { 1, 3 }, { 3, 44 }, { 5, 93 } };

    auto result1 = FROM (records1)
        JOIN (records2) ON (o1.x == o2.a)
        WHERE2 (o1.y % 100 == 0)
        ORDERBY2 (o1.y, DESCEND)
        SKIP (1)
        SELECT2 (o1.x, o1.y, o2.b);

    std::vector<std::tuple<int, int, int>> expectedResult1 = { { 1, 300, 3 }, { 1, 100, 3 } };
    EXPECT_EQ(result1, expectedResult1);

    Record3 records3[] = { { 0, 100 }, { 2, 22 }, { 3, 341 }, { 4, 965 } };

    auto result2 = FROM (records1)
        JOIN (records2) ON (o1.x == o2.a)
        WHERE2 (o1.x == 5)
        JOIN2 (records3) ON (o1.y == o3.n)
        ORDERBY3 (o1.y, DESCEND)
        TAKE (1)
        SELECT3 (o1.x, o1.y, o2.b, o3.m);
    
    std::vector<std::tuple<int, int, int, int>> expectedResult2 = { { 5, 965, 93, 4 } };
    EXPECT_EQ(result2, expectedResult2);
}

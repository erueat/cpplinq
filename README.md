# cpplinq

## Introduction

cpplinq is a tool aiming to simulate .NET Framework LINQ with c++.

With cpplinq, you will be able to use SQL-like grammar to do some search in an array.

For example, you can use codes below to search items whose x value is even,
order results with x descending, and get x and y^2 for each matched item.

```cpp
#define USE_CPPLINQ_MACRO
#include "cpplinq.h"

struct Record
{
    int x;
    int y;
};

std::vector<Record> records = { {1, 10}, {2, 12}, {3, 11}, {4, 13} };

auto result = CPPLINQ(Records)
    FROM (records)
    WHERE (o.x % 2 == 0)
    ORDERBY (o.x, DESCEND)
    SELECT (int, int) ON (o.x, o.y * o.y);
```

The result would be ```{ { 4, 169 }, { 2, 144 } }``` with type ```std::vector<std::tuple<int, int>>```.

## Supported Clauses

* where
* orderBy
* select
* take
* skip
* count
* first
* last
* sum
* average


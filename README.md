# cpplinq

cpplinq is a tool aiming to simulate LINQ in .NET Framework.

With cpplinq, you will be able to use SQL-like grammar to do some search in an array.

For example, you can use codes below to search items whose x value is even,
order results with x descending, and get x and y^2 for each matched item.

```cpp

struct Table
{
    int x;
    int y;
};

vector<Table> numbers = { {1, 10}, {2, 12}, {3, 11}, {4, 13} };

auto result = LINQ(Table)
    FROM (numbers)
    WHERE (o.x % 2 == 0)
    ORDERBY (o.x)
    SELECT (int, int) ON (o.x, o.y * o.y);
```

The result would be``` { { 4, 169 }, { 2, 144 } }```.

#include <tuple>
#include <vector>
#include <algorithm>

namespace zen
{
template <typename T>
class Linq
{
public:
    template <typename Array>
    Linq& from(Array& array)
    {
        for (T& element : array)
            m_array.push_back(element);
        return *this;
    }

    template <typename Condition>
    Linq& where(Condition condition)
    {
        std::vector<T> array;
        for (T element : m_array)
        {
            if (condition(element))
            {
                array.push_back(element);
            }
        }
        m_array = array;
        return *this;
    }

    template <typename GetOrderKey>    
    Linq& orderBy(GetOrderKey getOrderKey)
    {
        auto sortFunc = [&](const T& l, const T& r){ return (getOrderKey(l) > getOrderKey(r)); };
        std::sort(m_array.begin(), m_array.end(), sortFunc);
        return *this;
    }

    template <typename... Types, typename SelectFunc>
    std::vector<std::tuple<Types...>> select(SelectFunc selectFunc)
    {
        std::vector<std::tuple<Types...>> result;
        for (T& element : m_array)
        {
            result.push_back(selectFunc(element));
        }
        return result;
    }

private:
    std::vector<T> m_array;
};

#ifdef USE_LINQ_MACRO

#define LINQ(Type) Linq<Type>()
#define FROM(o) .from(o)
#define WHERE(condition) .where([](const auto& o) { return condition; })
#define ORDERBY(key) .orderBy([](const auto& o) { return key; })
#define SELECT(...) .select<__VA_ARGS__>
#define ON(...) ([](const auto& o) { return std::make_tuple(__VA_ARGS__); })

#endif
};

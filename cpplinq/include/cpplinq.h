#include <tuple>
#include <vector>
#include <algorithm>

namespace zen
{
template <typename T>
using DefaultCondition = bool(*)(const T&);

enum Order
{
    Ascend = 0,
    Descend = 1
};

template <typename T, typename Condition = DefaultCondition<T>>
class CppLinq
{
    class iterator
    {
    public:
        iterator(std::vector<T>& o, typename std::vector<T>::const_iterator it, Condition cond) : list(o), iter(it), condition(cond)
        {
            while (iter != list.end() && !condition(*iter))
            {
                iter++;
            }
        }

        iterator& operator++()
        {
            do
            {
                iter++;
            } while (iter != list.end() && !condition(*iter));
            return *this;
        }

        iterator& operator++(int)
        {
            iterator it(*this);
            do
            {
                iter++;
            } while (iter != list.end() && !condition(*iter));
            return it;
        }

        iterator operator+(size_t steps)
        {
            iterator it = *this;
            while(steps--)
            {
                ++it;
            }
            return it;
        }

        iterator operator-(size_t steps)
        {
            iterator it = *this;
            while (steps != 0 && it.iter != list.begin())
            {
                if (condition(*iter))
                {
                    steps--;
                }
                it.iter--;
            }
            return it;
        }

        size_t operator-(const iterator& r)
        {
            int count = 0;
            iterator it = r;
            while (it != *this)
            {
                ++it;
                count++;
            }
            return count;
        }

        bool operator==(const iterator& r)
        {
            return iter == r.iter;
        }

        bool operator!=(const iterator& r)
        {
            return iter != r.iter;
        }

        const T& operator*()
        {
            return *iter;
        }

    private:
        std::vector<T>& list;
        typename std::vector<T>::const_iterator iter;
        Condition condition;
    };
public:
    CppLinq()
    {
        m_condition = [](const T&) { return true; };
    }

    CppLinq(Condition& condition, std::vector<T>& v) : m_array(std::move(v)), m_condition(condition)
    {
    }

    CppLinq(const CppLinq& r) : m_condition(r.m_condition)
    {
        m_array = r.m_array;
    }

    CppLinq(CppLinq&& r) : m_condition(r.m_condition)
    {
        m_array = std::move(r.m_array);
    }

    template <typename Array>
    CppLinq& from(Array& array)
    {
        for (T& element : array)
            m_array.push_back(element);
        return *this;
    }

    CppLinq& take(size_t count)
    {
        m_takeCount = count;
        return *this;
    }

    CppLinq& skip(size_t count)
    {
        m_skipCount = count;
        return *this;
    }

    const T& first()
    {
        return *begin();
    }

    const T& last()
    {
        return *(end() - 1);
    }

    size_t count()
    {
        return end() - begin();
    }

    T sum()
    {
        T sum = 0;
        for (const T& element : *this)
        {
            sum += element;
        }
        return sum;
    }

    T average()
    {
        T sum = 0;
        int count = 0;
        for (const T& element : *this)
        {
            sum += element;
            count++;
        }
        return sum / (T)count;
    }

    template <typename Condition2>
    CppLinq<T, Condition2> where(Condition2 condition)
    {
        CppLinq<T, Condition2> linq(condition, m_array); 
        return linq;
    }

    template <typename GetOrderKey>    
    CppLinq& orderBy(GetOrderKey getOrderKey, Order order = Ascend)
    {
        auto sortFunc = [&](const T& l, const T& r){ return order == Ascend ? (getOrderKey(l) < getOrderKey(r)) : (getOrderKey(l) > getOrderKey(r)); };
        std::sort(m_array.begin(), m_array.end(), sortFunc);
        return *this;
    }

    template <typename SelectFunc>
    auto select(SelectFunc selectFunc)
    {
        using ReturnType = decltype(selectFunc(std::declval<const T&>()));
        std::vector<ReturnType> result;
        size_t count = m_takeCount;
        for (const T& ele : *this)
        {
            if (count--)
                result.push_back(selectFunc(ele));
            else
                break;
        }
        return result;
    }

protected:
    iterator begin()
    {
        return iterator(m_array, m_array.begin(), m_condition) + m_skipCount;
    }

    iterator end()
    {
        return iterator(m_array, m_array.end(), m_condition);
    }
private:
    std::vector<T> m_array;
    size_t m_skipCount = 0;
    size_t m_takeCount = SIZE_T_MAX;
    Condition m_condition;
};
};

#ifdef USE_CPPLINQ_MACRO

#define CPPLINQ(Type) zen::CppLinq<Type>()
#define FROM(o) .from(o)
#define WHERE(condition) .where([](const auto& o) { return condition; })
#define ORDERBY(key, Args...) .orderBy([](const auto& o) { return key; }, ##Args)
#define DESCEND zen::Descend
#define TAKE(count) .take(count)
#define SKIP(count) .skip(count)
#define FIRST() .first()
#define LAST() .last()
#define COUNT() .count()
#define SUM() .sum()
#define AVERAGE() .average()
#define SELECT(...) .select([](const auto& o) { return std::make_tuple(__VA_ARGS__); })

#endif

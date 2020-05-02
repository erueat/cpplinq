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

template <typename IteratorType>
using ElementType = typename std::decay<decltype(*std::declval<IteratorType>())>::type;

template <typename IteratorType, typename Condition = DefaultCondition<ElementType<IteratorType>>>
class CppLinq
{
    using T = ElementType<IteratorType>;
    class iterator
    {
    public:
        iterator(IteratorType begin, IteratorType end, IteratorType it, Condition cond) 
            : m_begin(begin), m_end(end), m_iter(it), m_condition(cond)
        {
            while (m_iter != m_end && !m_condition(*m_iter))
            {
                m_iter++;
            }
        }

        iterator& operator++()
        {
            do
            {
                m_iter++;
            } while (m_iter != m_end && !m_condition(*m_iter));
            return *this;
        }

        iterator& operator++(int)
        {
            iterator it(*this);
            do
            {
                m_iter++;
            } while (m_iter != m_end && !m_condition(*m_iter));
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
            while (steps != 0 && it.m_iter != m_begin)
            {
                if (m_condition(*m_iter))
                {
                    steps--;
                }
                it.m_iter--;
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
            return m_iter == r.m_iter;
        }

        bool operator!=(const iterator& r)
        {
            return m_iter != r.m_iter;
        }

        const T& operator*()
        {
            return *m_iter;
        }

    private:
        IteratorType m_begin;
        IteratorType m_end;
        IteratorType m_iter;
        Condition m_condition;
    };
public:
    CppLinq(IteratorType begin, IteratorType end): m_begin(begin), m_end(end)
    {
        m_condition = [](const T&) { return true; };
    }

    CppLinq(Condition& condition, IteratorType begin, IteratorType end) : m_begin(begin), m_end(end), m_condition(condition)
    {
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
    CppLinq<IteratorType, Condition2> where(Condition2 condition)
    {
        CppLinq<IteratorType, Condition2> linq(condition, m_begin, m_end); 
        return linq;
    }

    template <typename GetOrderKey>    
    CppLinq& orderBy(GetOrderKey getOrderKey, Order order = Ascend)
    {
        auto sortFunc = [&](const T& l, const T& r){ return order == Ascend ? (getOrderKey(l) < getOrderKey(r)) : (getOrderKey(l) > getOrderKey(r)); };
        std::sort(m_begin, m_end, sortFunc);
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
        return iterator(m_begin, m_end, m_begin, m_condition) + m_skipCount;
    }

    iterator end()
    {
        return iterator(m_begin, m_end, m_end, m_condition);
    }
private:
    IteratorType m_begin;
    IteratorType m_end;
    size_t m_skipCount = 0;
    size_t m_takeCount = SIZE_T_MAX;
    Condition m_condition;
};

template <typename IteratorType>
auto from(IteratorType begin, IteratorType end)
{
    return CppLinq(begin, end);
}
};

#ifdef USE_CPPLINQ_MACRO

#define FROM(o) zen::from(std::begin(o), std::end(o))
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

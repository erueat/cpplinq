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

template <typename ContainerType>
using ElementType = typename std::decay<decltype(*(std::begin(std::declval<ContainerType>())))>::type;

template <typename ContainerType, typename Condition = DefaultCondition<ElementType<ContainerType>>>
class CppLinq
{
    using T = ElementType<ContainerType>;
    class iterator
    {
        using IteratorType = decltype(std::begin(std::declval<ContainerType>()));
    public:
        iterator(ContainerType& o, IteratorType it, Condition cond) : container(o), iter(it), condition(cond)
        {
            while (iter != std::end(container) && !condition(*iter))
            {
                iter++;
            }
        }

        iterator& operator++()
        {
            do
            {
                iter++;
            } while (iter != std::end(container) && !condition(*iter));
            return *this;
        }

        iterator& operator++(int)
        {
            iterator it(*this);
            do
            {
                iter++;
            } while (iter != std::end(container) && !condition(*iter));
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
            while (steps != 0 && it.iter != std::begin(container))
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
        ContainerType& container;
        IteratorType iter;
        Condition condition;
    };
public:
    CppLinq(ContainerType& container): m_container(container)
    {
        m_condition = [](const T&) { return true; };
    }

    CppLinq(Condition& condition, ContainerType& container) : m_container(container), m_condition(condition)
    {
    }

    CppLinq(const CppLinq& r) : m_container(r.m_container), m_condition(r.m_condition)
    {
    }

    CppLinq(CppLinq&& r) : m_container(r.m_container), m_condition(r.m_condition)
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
    CppLinq<ContainerType, Condition2> where(Condition2 condition)
    {
        CppLinq<ContainerType, Condition2> linq(condition, m_container); 
        return linq;
    }

    template <typename GetOrderKey>    
    CppLinq& orderBy(GetOrderKey getOrderKey, Order order = Ascend)
    {
        auto sortFunc = [&](const T& l, const T& r){ return order == Ascend ? (getOrderKey(l) < getOrderKey(r)) : (getOrderKey(l) > getOrderKey(r)); };
        std::sort(m_container.begin(), m_container.end(), sortFunc);
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
        return iterator(m_container, std::begin(m_container), m_condition) + m_skipCount;
    }

    iterator end()
    {
        return iterator(m_container, std::end(m_container), m_condition);
    }
private:
    ContainerType& m_container;
    size_t m_skipCount = 0;
    size_t m_takeCount = SIZE_T_MAX;
    Condition m_condition;
};

template <typename ContainerType>
auto from(ContainerType& container)
{
    return CppLinq<ContainerType>(container);
}
};

#ifdef USE_CPPLINQ_MACRO

#define FROM(o) zen::from(o)
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

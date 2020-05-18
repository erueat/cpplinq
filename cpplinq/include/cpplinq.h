#include <tuple>
#include <vector>
#include <algorithm>


namespace zen
{

template <typename IterType>
using ElementType = typename std::decay<decltype(*std::declval<IterType>())>::type;

template <typename EleType>
using IteratorType = typename std::decay<decltype(std::begin(std::declval<std::vector<EleType>>()))>::type;

template <typename T>
using DefaultCondition = bool(*)(const T&);

template <typename IterType, typename Condition>
class iterator
{
public:
    iterator(IterType begin, IterType end, IterType it, Condition cond) 
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

    const ElementType<IterType>& operator*()
    {
        return *m_iter;
    }

private:
    IterType m_begin;
    IterType m_end;
    IterType m_iter;
    Condition m_condition;
};

enum Order
{
    Ascend = 0,
    Descend = 1
};

template <typename... Types>
class CppLinq;

template <typename... Types>
struct Data;

template <typename IterType, typename RealType, typename WhereCondition = DefaultCondition<ElementType<IterType>>>
class Base
{
public:
    Base(WhereCondition condition) : m_condition(condition) {}

    Base(IterType begin, IterType end) : m_condition([](const auto&){ return true; })
    {
        m_begin = begin;
        m_end = end;
    }

    Base(IterType begin, IterType end, WhereCondition condition) : m_condition(condition)
    {
        m_begin = begin;
        m_end = end;
    }

    const auto& first()
    {
        return *begin();
    }

    const auto& last()
    {
        return *(end() - 1);
    }

    size_t count()
    {
        return end() - begin();
    }

    auto sum()
    {
        ElementType<IterType> sum = 0;
        for (const auto& element : *this)
        {
            sum += element;
        }
        return sum;
    }

    auto average()
    {
        ElementType<IterType> sum = 0;
        int count = 0;
        for (const auto& element : *this)
        {
            sum += element;
            count++;
        }
        return sum / (ElementType<IterType>)count;
    }

    auto take(size_t count)
    {
        m_takeCount = count;
        return *(RealType*)this;
    }

    auto skip(size_t count)
    {
        m_skipCount = count;
        return *(RealType*)this;
    }

    iterator<IterType, WhereCondition> begin()
    {
        return iterator<IterType, WhereCondition>(m_begin, m_end, m_begin, m_condition) + m_skipCount;
    }

    iterator<IterType, WhereCondition> end()
    {
        return iterator<IterType, WhereCondition>(m_begin, m_end, m_end, m_condition);
    }

protected:
    IterType m_begin;
    IterType m_end;
    WhereCondition m_condition;
    size_t m_takeCount = SIZE_MAX;
    size_t m_skipCount = 0;
};

template <typename T1, typename T2, typename T3, typename T4>
struct Data<T1, T2, T3, T4>
{
    T1 var1;
    T2 var2;
    T3 var3;
    T4 var4;
};

template <typename T1, typename T2, typename T3, typename T4, typename WhereCondition>
class CppLinq<T1, T2, T3, T4, WhereCondition> : public Base<
    IteratorType<Data<T1,T2,T3,T4>>,
    CppLinq<T1, T2, T3, T4, WhereCondition>,
    WhereCondition>
{
    using super = Base<
        IteratorType<Data<T1,T2,T3,T4>>,
        CppLinq<T1, T2, T3, T4, WhereCondition>,
        WhereCondition>;
private:
    std::vector<Data<T1, T2, T3, T4>> m_data;
public:
    CppLinq() : super([](const auto&){return true;})
    {
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    void addData(const Data<T1, T2, T3, T4>& v)
    {
        m_data.push_back(v);
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    template <typename Condition2>
    CppLinq<T1, T2, T3, T4, Condition2> where(Condition2 condition)
    {
        CppLinq<T1, T2, T3, T4, Condition2> linq(m_data, condition);
        return linq;
    }

    template <typename GetOrderKey>    
    auto& orderBy(GetOrderKey getOrderKey, Order order = Order::Ascend)
    {
        auto sortFunc = [&](const auto& l, const auto& r){ 
                auto lKey = getOrderKey(l.var1, l.var2, l.var3, l.var4);
                auto rKey = getOrderKey(r.var2, r.var2, r.var3, r.var4);
                return order == Order::Ascend ? lKey < rKey : lKey > rKey;
            };
        std::sort(super::m_begin, super::m_end, sortFunc);
        return *this;
    }

    template <typename SelectFunc>
    auto select(SelectFunc selectFunc)
    {
        using ReturnType = decltype(selectFunc(std::declval<const T1&, const T2&, const T3&, const T4&>()));
        std::vector<ReturnType> result;
        size_t count = super::m_takeCount;
        for (const auto& ele : *this)
        {
            if (count--)
                result.push_back(selectFunc(ele.var1, ele.var2, ele.var3, ele.var4));
            else
                break;
        }
        return result;
    }
};

template <typename T1, typename T2, typename T3>
struct Data<T1, T2, T3>
{
    T1 var1;
    T2 var2;
    T3 var3;
};

template <typename T1, typename T2, typename T3, typename WhereCondition>
class CppLinq<T1, T2, T3, WhereCondition> : public Base<
        IteratorType<Data<T1, T2, T3>>,
        CppLinq<T1, T2, T3, WhereCondition>,
        WhereCondition>
{
    using super = Base<
        IteratorType<Data<T1, T2, T3>>,
        CppLinq<T1, T2, T3, WhereCondition>,
        WhereCondition>;
private:
    std::vector<Data<T1, T2, T3>> m_data;
public:
    CppLinq() : super([](const auto&){return true;})
    {
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    CppLinq(std::vector<Data<T1, T2, T3>>& v) : super([](const auto&){return true;})
    {
        std::swap(m_data, v);
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    void addData(const Data<T1, T2, T3>& v)
    {
        m_data.push_back(v);
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    template <typename IterType, typename Condition>
    auto join(IterType begin, IterType end, Condition condition)
    {
        using T4 = ElementType<IterType>;
        using NewType = CppLinq<T1, T2, T3, T4> ;
        NewType result;
        for (const auto& ele : m_data)
        {
            for (IterType it = begin; it != end; it++)
            {
                const T4& ele4 = *it;
                if (condition(ele.var1, ele.var2, ele.var3, ele4))
                {
                    result.addData({ ele.var1, ele.var2, ele.var3, ele4 });
                }
            }
        }
        return result;
    }

    template <typename Condition2>
    CppLinq<T1, T2, T3, Condition2> where(Condition2 condition)
    {
        CppLinq<T1, T2, T3, Condition2> linq(m_data, condition);
        return linq;
    }

    template <typename GetOrderKey>    
    auto& orderBy(GetOrderKey getOrderKey, Order order = Order::Ascend)
    {
        auto sortFunc = [&](const auto& l, const auto& r){ 
                auto lKey = getOrderKey(l.var1, l.var2, l.var3);
                auto rKey = getOrderKey(r.var2, r.var2, r.var3);
                return order == Order::Ascend ? lKey < rKey : lKey > rKey;
            };
        std::sort(super::m_begin, super::m_end, sortFunc);
        return *this;
    }

    template <typename SelectFunc>
    auto select(SelectFunc selectFunc)
    {
        using ReturnType = decltype(selectFunc(std::declval<const T1&>(), std::declval<const T2&>(), std::declval<const T3&>()));
        std::vector<ReturnType> result;
        size_t count = super::m_takeCount;
        for (const auto& ele : *this)
        {
            if (count--)
                result.push_back(selectFunc(ele.var1, ele.var2, ele.var3));
            else
                break;
        }
        return result;
    }
};

template <typename T1, typename T2>
struct Data<T1, T2>
{
    T1 var1;
    T2 var2;
};
    
template <typename T1, typename T2, typename WhereCondition>
class CppLinq<T1, T2, WhereCondition> 
    : public Base<
        IteratorType<Data<T1, T2>>,
        CppLinq<T1, T2, WhereCondition>,
        WhereCondition>
{
    using super = Base<
        IteratorType<Data<T1, T2>>,
        CppLinq<T1, T2, WhereCondition>,
        WhereCondition>;
private:
    std::vector<Data<T1, T2>> m_data;
public:
    CppLinq() : super([](const auto&){return true;})
    {
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    CppLinq(std::vector<Data<T1, T2>>& v) : super([](const auto&){return true;})
    {
        std::swap(m_data, v);
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }

    CppLinq(IteratorType<Data<T1, T2>> begin, IteratorType<Data<T1, T2>> end, WhereCondition condition) : super(begin, end, condition) {}

    void addData(const Data<T1, T2>& v)
    {
        m_data.push_back(v);
        super::m_begin = m_data.begin();
        super::m_end = m_data.end();
    }
    
    template <typename IterType, typename Condition>
    auto join(IterType begin, IterType end, Condition condition)
    {
        using T3 = ElementType<IterType>;
        CppLinq<T1, T2, T3, DefaultCondition<Data<T1, T2, T3>>> result;
        for (const auto& ele : m_data)
        {
            for (auto it = begin; it != end; it++)
            {
                const T3& ele3 = *it;
                if (condition(ele.var1, ele.var2, ele3))
                {
                    result.addData({ ele.var1, ele.var2, ele3 });
                }
            }
        }
        return result;
    }

    template <typename Condition2>
    CppLinq<T1, T2, Condition2> where(Condition2 condition)
    {
        CppLinq<T1, T2, Condition2> linq(m_data, condition);
        return linq;
    }

    template <typename GetOrderKey>    
    auto& orderBy(GetOrderKey getOrderKey, Order order = Order::Ascend)
    {
        auto sortFunc = [&](const auto& l, const auto& r){ 
                auto lKey = getOrderKey(l.var1, l.var2);
                auto rKey = getOrderKey(r.var2, r.var2);
                return order == Order::Ascend ? lKey < rKey : lKey > rKey;
            };
        std::sort(super::m_begin, super::m_end, sortFunc);
        return *this;
    }

    template <typename SelectFunc>
    auto select(SelectFunc selectFunc)
    {
        using ReturnType = decltype(selectFunc(std::declval<const T1&>(), std::declval<const T2&>()));
        std::vector<ReturnType> result;
        size_t count = super::m_takeCount;
        for (const auto& ele : *this)
        {
            if (count--)
                result.push_back(selectFunc(ele.var1, ele.var2));
            else
                break;
        }
        return result;
    }
    
};

template <typename IterType, typename WhereCondition>
class CppLinq<IterType, WhereCondition> : public Base<IterType, CppLinq<IterType, WhereCondition>, WhereCondition>
{
    using super = Base<IterType, CppLinq<IterType, WhereCondition>, WhereCondition>;
public:
    CppLinq(IterType begin, IterType end) : super(begin, end) {}
    CppLinq(IterType begin, IterType end, WhereCondition condition) : super(begin, end, condition) {}
    
    template <typename IterType2, typename JoinCondition>
    auto join(IterType2 begin2, IterType2 end2, JoinCondition condition)
    {
        using T1 = ElementType<IterType>;
        using T2 = ElementType<IterType2>;
        CppLinq<T1, T2, DefaultCondition<Data<T1, T2>>> result;
        for (IterType it1 = super::m_begin; it1 != super::m_end; ++it1)
        {
            const T1& ele = *it1;
            for (IterType2 it2 = begin2; it2 != end2; ++it2)
            {
                const T2& ele2 = *it2;
                if (condition(ele, ele2))
                {
                    result.addData({ ele, ele2 });
                }
            }
        }
        return result;
    }

    template <typename WhereCondition2>
    CppLinq<IterType, WhereCondition2> where(WhereCondition2 condition)
    {
        CppLinq<IterType, WhereCondition2> linq(super::m_begin, super::m_end, condition); 
        return linq;
    }

    template <typename GetOrderKey>    
    auto& orderBy(GetOrderKey getOrderKey, Order order = Order::Ascend)
    {
        auto sortFunc = [&](const auto& l, const auto& r){ 
                auto lKey = getOrderKey(l);
                auto rKey = getOrderKey(r);
                return order == Order::Ascend ? lKey < rKey : lKey > rKey;
            };
        std::sort(super::m_begin, super::m_end, sortFunc);
        return *this;
    }

    template <typename SelectFunc>
    auto select(SelectFunc selectFunc)
    {
        using T = ElementType<IterType>;
        using ReturnType = decltype(selectFunc(std::declval<const T&>()));
        std::vector<ReturnType> result;
        size_t count = super::m_takeCount;
        for (const auto& ele : *this)
        {
            if (count--)
                result.push_back(selectFunc(ele));
            else
                break;
        }
        return result;
    }
};

template <typename IterType>
auto from(IterType begin, IterType end)
{
    return CppLinq<IterType, DefaultCondition<ElementType<IterType>>>(begin, end);
}
};

#ifdef USE_CPPLINQ_MACRO

#define FROM(o) zen::from(std::begin(o), std::end(o))
#define WHERE(condition) .where([](const auto& o) { return condition; })
#define VA_ARGS(...) , ##__VA_ARGS__
#define ORDERBY(key, ...) .orderBy([](const auto& o) { return key; } VA_ARGS(__VA_ARGS__))
#define DESCEND zen::Order::Descend
#define TAKE(count) .take(count)
#define SKIP(count) .skip(count)
#define FIRST() .first()
#define LAST() .last()
#define COUNT() .count()
#define SUM() .sum()
#define AVERAGE() .average()

#define SELECT(...) .select([](const auto& o) { return std::make_tuple(__VA_ARGS__); })
#define SELECT2(...) .select([](const auto& o1, const auto& o2) { return std::make_tuple(__VA_ARGS__); })
#define SELECT3(...) .select([](const auto& o1, const auto& o2, const auto& o3) { return std::make_tuple(__VA_ARGS__); })

#define JOIN(o) .join(std::begin(o), std::end(o), [](const auto& o1, const auto& o2)
#define JOIN2(o) .join(std::begin(o), std::end(o), [](const auto& o1, const auto& o2, const auto& o3)

#define ON(...)  { return __VA_ARGS__; })

#endif

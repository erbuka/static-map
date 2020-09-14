// Type your code here, or load an example.

#include <utility>
#include <type_traits>
#include <iostream>
#include <vector>



template<auto A, auto B>
struct IsEqual {
    static constexpr bool Value = false;
};

template<typename T, T A, T B>
struct IsEqual<A, B> {
    static constexpr bool Value = A == B;
};

template<auto K, typename V>
struct Entry {
    using KeyType = decltype(K);
    using ValueType = V;
    static constexpr KeyType Key = K;
    ValueType Value;
};

template<auto K, typename V>
constexpr Entry<K, V> MakeEntry(V&& v) { return Entry<K, V>{ std::forward<V>(v) }; }


template<typename Entry, typename... Args>
class StaticMap
{
public:
    constexpr StaticMap(Entry&& entry, Args&&... args) :
        m_Entry(std::forward<Entry>(entry)),
        m_Tail(std::forward<Args>(args)...) 
    { }

    template<auto K>
    const auto& Get() const
    {
        if constexpr(IsEqual<K, Entry::Key>::Value) return m_Entry.Value;
        else return m_Tail.template Get<K>();
    }

    template<auto K>
    auto& Get()
    {
        if constexpr(IsEqual<K, Entry::Key>::Value) return m_Entry.Value;
        else return m_Tail.template Get<K>();
    }

private:
    Entry m_Entry;
    StaticMap<Args...> m_Tail;
};


template<typename Entry>
class StaticMap<Entry>
{
public:
    constexpr StaticMap(Entry&& entry) :
        m_Entry(std::forward<Entry>(entry))
    {
        
    }

    template<auto K>
    const auto& Get() const
    {
        if constexpr(IsEqual<K, Entry::Key>::Value) return m_Entry.Value;
        else throw std::range_error("Not found");
    }

    template<auto K>
    auto& Get()
    {
        if constexpr(IsEqual<K, Entry::Key>::Value) return m_Entry.Value;
        else throw std::range_error("Not found");
    }

private:
    Entry m_Entry;
};

enum class E { V0, V1, V2, V3 };

int main() {
    StaticMap myMap(
        MakeEntry<1>(100),
        MakeEntry<E::V1>(std::vector<double>()),
        MakeEntry<2>(300.0f),
        MakeEntry<E::V3>(std::make_pair(1, 2))
    );

    std::cout << myMap.Get<1>() << std::endl;
    std::cout << myMap.Get<E::V1>().size() << std::endl;
    std::cout << myMap.Get<2>() << std::endl;
    std::cout << myMap.Get<E::V3>().second << std::endl;
}
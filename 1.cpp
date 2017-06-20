#include <string>
#include <vector>
#include <algorithm>

#include <set>
#include <assert.h>
#include <iostream>
#include <sstream>

//#include "mp11.hpp"

namespace mp11 {

using mp_true = std::integral_constant<bool, true>;
using mp_false = std::integral_constant<bool, false>;

template<class...> struct voider {
    using type = void;
};

template<class... T>
using void_t = typename voider<T...>::type;

// template<class...>
// using void_t = void;

#define mp_hasfn_with_signature_declare(FNNAME)                         \
    template<typename, typename T>                                      \
    struct mp_hasfn_with_signature_##FNNAME##_t {                       \
        static_assert(                                                  \
            std::integral_constant<T, false>::value,                    \
            "Second template parameter needs to be of function type."); \
    };                                                                  \
                                                                        \
    template<typename C, typename Ret, typename... Args>                \
    struct mp_hasfn_with_signature_##FNNAME##_t<C, Ret(Args...)> {      \
    private:                                                            \
    template<typename T>                                                \
    static inline constexpr auto check(T*)                              \
        -> typename                                                     \
        std::is_same<decltype(std::declval<T>().FNNAME(std::declval<Args>()... )), \
                     Ret                                                \
                     >::type                                            \
        {return {};}                                                    \
                                                                        \
    template<typename>                                                  \
    static inline constexpr std::false_type check(...){return {};}      \
                                                                        \
    typedef decltype(check<C>(0)) type;                                 \
                                                                        \
    public:                                                             \
    static constexpr bool value = type::value;                          \
    };                                                                  \
    template<class... T>                                                \
    inline constexpr bool mp_hasfn_with_signature_##FNNAME() {          \
        return mp_hasfn_with_signature_##FNNAME##_t<T...>::value;       \
    }

#define mp_hasfn_with_signature(CLASS, FNNAME, FN)  \
    mp_hasfn_with_signature_##FNNAME<CLASS, FN>()

#define mp_hasfn_with_params_declare(FNNAME)                            \
    template<typename C, typename... Args>                              \
    struct mp_hasfn_with_params_##FNNAME##_t {                          \
    private:                                                            \
    template<class T, class = void>                                     \
    struct mp_hasfunction_ : mp_false {};                               \
                                                                        \
    template<class T>                                                   \
    struct mp_hasfunction_<T,                                           \
                           void_t<                                      \
                              decltype(                                 \
                                  std::declval<T&>().FNNAME(std::declval<Args>()... ))>> \
        : mp_true {};                                                   \
                                                                        \
    public:                                                             \
    using type = typename mp_hasfunction_<C>::type;                     \
    static constexpr bool value = type::value;                          \
    };                                                                  \
    template<class... T>                                                \
    constexpr bool mp_hasfn_with_params_##FNNAME() {                    \
        return mp_hasfn_with_params_##FNNAME##_t<T...>::value;          \
    }

#define mp_hasfn_with_params(CLASS, FNNAME, FN)  \
    mp_hasfn_with_params_##FNNAME<CLASS, FN>()

} /* namespace mp11 */


using namespace mp11;

namespace utils {

template<class T>
using ContainerElemT = typename std::decay<decltype(*std::begin(std::declval<T&>()))>::type;


mp_hasfn_with_params_declare(push_back);
mp_hasfn_with_params_declare(push_front);
mp_hasfn_with_params_declare(insert);
mp_hasfn_with_params_declare(erase);
mp_hasfn_with_params_declare(pop_front);
mp_hasfn_with_params_declare(pop_back);
mp_hasfn_with_params_declare(reserve);
mp_hasfn_with_signature_declare(to_string);
mp_hasfn_with_signature_declare(toString);
mp_hasfn_with_signature_declare(at);
mp_hasfn_with_signature_declare(data);
mp_hasfn_with_params_declare(begin);

namespace nocopy {


template<class T, class V>
auto push_back(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, push_back, ContainerElemT<T>),
        void
    >::type
{
    list.push_back(std::forward<V>(value));
}


template<class T, class V>
auto push_back(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, insert, ContainerElemT<T>),
        void
    >::type
{
    list.insert(std::forward<V>(value));
}


template<class T, class V>
auto push_front(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, push_front, ContainerElemT<T>),
        void
    >::type
{
    list.push_front(std::forward<V>(value));
}


template<class T, class V>
auto push_front(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params_insert<T, decltype(std::begin(list)), ContainerElemT<T>>(),
        void
    >::type
{
    list.insert(std::begin(list), std::forward<V>(value));
}


template<class T, class V>
auto insert(T& list, const std::size_t i, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params_insert<T, decltype(std::begin(list)), ContainerElemT<T>>(),
        void
    >::type
{
    if (i == 0) {
        push_front(list, std::forward<V>(value));
    }
    else if (i >= list.size()) {
        push_back(list, std::forward<V>(value));
    }
    else {
        auto it = std::begin(list);
        std::advance(it, std::min(i, list.size()));
        list.insert(it, std::forward<V>(value));
    }
}


template<class T>
auto remove_front(T& list) ->
    typename std::enable_if<
    mp_hasfn_with_params(T, pop_front, void),
        void
    >::type
{
    list.pop_front();
}


template<class T>
auto remove_front(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    list.erase(std::begin(list));
}


template<class T>
auto remove_back(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, pop_back, void),
        void
    >::type
{
    list.pop_back();
}


template<class T>
auto remove_back(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    list.erase(std::end(list));
}


template<class T>
auto remove_n(T& list, const std::size_t i) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    if (i >= list.size()) {
        return;
    }
    auto it = std::begin(list);
    std::advance(it, std::min(i, list.size()));
    list.erase(it);
}


template<class T, class ValueT>
auto remove(T& list, const ValueT& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    auto it = std::begin(list);
    while (it != std::end(list) and *it != value) {
        ++it;
    }
    if (it != std::end(list)) {
        list.erase(it);
    }
}


template<class T, class SizeT>
auto reserve(T&, const SizeT) ->
    typename std::enable_if<
        !mp_hasfn_with_params(T, reserve, SizeT),
        void
    >::type
{

}


template<class T, class SizeT>
auto reserve(T& list, const SizeT value) -> decltype(list.reserve(value), (void)0)
{
    list.reserve(value);
}


template<class T, class Fn>
auto filter(Fn fn, T& list) -> void
{
    list.erase(std::copy_if(
                   std::begin(list),
                   std::end(list),
                   std::begin(list),
                   std::forward<Fn>(fn)),
               std::end(list));
}


template<class T>
auto sort(T& list) -> void
{
    std::sort(
        std::begin(list),
        std::end((list)));
}


template<class T, class Comparator>
auto sort(Comparator&& comparator, T& list) -> void
{
    std::sort(
        std::begin(list),
        std::end(list),
        std::forward<Comparator>(comparator));
}


template<class Fn, class T>
auto map(Fn fn, T& list) -> void
{
    list.erase(
        std::transform(std::begin(list), std::end(list), std::begin(list), fn),
        std::end(list));
}


template<class Fn, template<class...> class T, class... U>
auto foreach(Fn fn, const T<U...>& list) -> void
{
    for (const auto& elem : list) {
        fn(elem);
    }
}


template<class T>
auto take(T& list, const std::size_t N) -> void
{
    list.erase(std::copy_n(
                   std::begin(list),
                   N,
                   std::begin(list)),
               std::end(list));
}


template<class T, class Fn>
auto take_while(T& list, Fn fn) -> void
{
    auto it = std::begin(list);

    for (const auto& elem : list) {
        if (not fn(elem)) {
            break;
        }

        ++it;
    }

    list.erase(it, std::end(list));
}


template<class T>
auto drop(T& list, const std::size_t N) -> void
{
    auto it = std::begin(list);

    std::size_t i = 0;
    while (it != std::end(list) and i++ < N) {
        ++it;
    }

    list.erase(std::begin(list), it);
}


template<class T, class Fn>
auto drop_while(T& list, Fn fn) -> void
{
    auto it = std::begin(list);

    while (it != std::end(list) and fn(*it)) {
        ++it;
    }

    list.erase(std::begin(list), it);
}


template<class T>
auto tail(T& list) -> void
{
    remove_front(list);
}


template<class T>
auto slice(T& list, std::int64_t i) -> void
{
    if (list.empty()) {
        return;
    }

    if (i < 0) {
        if (static_cast<std::size_t>(-i) >= list.size()) {
            i = 0;
        }
        else {
            i = list.size() - -i;
        }
    }

    auto it = std::begin(list);
    std::advance(it, i);
    list.erase(std::begin(list), it);
}


template<class T>
auto slice(T& list, std::int64_t i, std::int64_t j) -> void
{
    if (list.empty()) {
        return;
    }

    if (i < 0) {
        if (static_cast<std::size_t>(-i) >= list.size()) {
            i = 0;
        }
        else {
            i = list.size() - -i;
        }
    }
    else {
        if (static_cast<std::size_t>(i) >= list.size()) {
            return;
        }
    }

    if (j < 0) {
        if (static_cast<std::size_t>(-j) >= list.size()) {
            return;
        }
        else {
            j = list.size() - -j;
        }
    }
    else {
        if (static_cast<std::size_t>(j) >= list.size()) {
            j = list.size();
        }
    }

    if (i > j) {
        return;
    }

    auto it = std::begin(list);
    std::advance(it, i);
    list.erase(std::begin(list), it);

    it = std::begin(list);
    std::advance(it, j - i);
    list.erase(it, std::end(list));
}


} // namespace nocopy


template<class T, class V>
auto push_back(const T& list, V&& value) -> T
{
    T result{list};
    nocopy::push_back(result, std::forward<V>(value));
    return result;
}


template<class T, class V>
auto push_front(const T& list, V&& value) -> T
{
    T result{list};
    nocopy::push_front(result, std::forward<V>(value));
    return result;
}


template<class T, class V>
auto insert(const T& list, const std::size_t i, V&& value) -> T
{
    T result{list};
    nocopy::insert(result, i, std::forward<V>(value));
    return result;
}


template<class T>
auto remove_n(const T& list, const std::size_t i) -> T
{
    T result{list};
    nocopy::remove_n(result, i);
    return result;
}


template<class T, class ValueT>
auto remove(const T& list, const ValueT& value) -> T
{
    T result{list};
    nocopy::remove(result, value);
    return result;
}


template<class... Args>
auto reserve(Args&& ...args) -> void
{
    nocopy::reserve(std::forward<Args>(args)...);
}


template<class FilterFn, class T>
auto filter(FilterFn filter, const T& list) -> typename std::decay<T>::type
{
    typename std::decay<T>::type result;
    reserve(result, list.size());

    for (const auto& elem : list) {
        if (filter(elem)) {
            nocopy::push_back(result, elem);
        }
    }

    return result;
}


template<class FilterFn, class T>
auto filter(FilterFn filter, T&& list) -> typename std::decay<T>::type
{
    typename std::decay<T>::type result;
    reserve(result, list.size());

    for (auto&& elem : list) {
        if (filter(elem)) {
            nocopy::push_back(result, std::move(elem));
        }
    }

    return result;
}


template<class T>
auto sort(T&& list) -> typename std::decay<T>::type
{
    typename std::decay<T>::type result(std::forward<T>(list));
    nocopy::sort(result);
    return result;
}


template<class T, class Comparator>
auto sort(Comparator&& comparator, T&& list) -> typename std::decay<T>::type
{
    typename std::decay<T>::type result(std::forward<T>(list));
    nocopy::sort(std::forward<Comparator>(comparator), result);
    return result;
}


template<class ResultT, class T>
auto convert(T&& list) -> ResultT
{
    return static_cast<ResultT>(std::forward<T>(list));
}


template<class Fn, template<class...> class T, class... U>
auto map(Fn fn, const T<U...>& list) -> T<decltype(fn(*std::begin(list)))>
{
    T<decltype(fn(*std::begin(list)))> result;
    for (const auto& elem : list) {
        nocopy::push_back(result, fn(elem));
    }
    return result;
}


template<template<class...> class T, class... Args, class ValueT>
void combine_inner(T<Args...>& result, const ValueT& value) {
    nocopy::push_back(result, value);
}


template<template<class...> class T1, class... Args1,
         template<class...> class T2, class... Args2>
auto combine_inner(T1<Args1...>& result, const T2<Args2...>& list1) ->
    typename std::enable_if<
        std::is_class<std::string>::value
        >::type
{
    for (const auto& elem : list1) {
        nocopy::push_back(result, elem);
    }
}


template<class T1, class T2, class... Args>
void combine_inner(T1& result, const T2& list1, Args&& ...args)
{
    combine_inner(result, list1);
    combine_inner(result, std::forward<Args>(args)...);
}


template<class... Args>
void combine_inner(std::string& result)
{

}


template<class... Args>
void combine_inner(std::string& result, const std::string& value, Args&& ...args)
{
    result += value;
    combine_inner(result, std::string(args)...);
}


template<class T, class... Args>
auto combine(const T& list1, Args&& ...args) -> T
{
    T result;
    combine_inner(result, list1, std::forward<Args>(args)...);
    return result;
}


template<class... Args>
auto combine(const std::string& list1, Args&& ...args) -> std::string
{
    std::string result;
    combine_inner(result, list1, std::forward<Args>(args)...);
    return result;
}


template<template<class...> class T, class ValueT, class... L>
auto contains(const T<ValueT, L...>& list, const ValueT& value) -> bool
{
    for (const auto& elem : list) {
        if (elem == value) {
            return true;
        }
    }
    return false;
}


template<class T, class ValueT>
auto find(T&& list, const ValueT& value) -> ValueT*
{
    for (const auto& elem : list) {
        if (elem == value) {
            return const_cast<ValueT*>(&elem);
        }
    }
    return nullptr;
}


template<template<class...> class T, class ValueT, class... L, class FindFn>
auto find_if(FindFn fn, const T<ValueT, L...>& list) -> ValueT*
{
    for (const auto& elem : list) {
        if (fn(elem)) {
            return const_cast<ValueT*>(&elem);
        }
    }
    return nullptr;
}


template<class T, class ValueT>
auto get(const T& list, const ValueT& value) -> ValueT&
{
    for (const auto& elem : list) {
        if (elem == value) {
            return *const_cast<ValueT*>(&elem);
        }
    }
    throw std::runtime_error("Value does not exists in list.");
}


template<class T>
auto nth(const T& list, const std::size_t N) ->
    typename std::decay<decltype(*std::begin(list))>::type&
{
    std::size_t i = 0;
    for (const auto& elem : list) {
        if (i < N) {
            ++i;
            continue;
        }
        return const_cast<typename std::decay<decltype(*std::begin(list))>::type&>(elem);
    }
    throw std::runtime_error("nth: wrong element position.");
}


template<template<class...> class T, class ValueT, class... L, class FindFn>
auto get_if(FindFn fn, const T<ValueT, L...>& list) -> ValueT&
{
    for (const auto& elem : list) {
        if (fn(elem)) {
            return *const_cast<ValueT*>(&elem);
        }
    }
    throw std::runtime_error("Value does not exists in list.");
}


template<typename Container, typename T>
auto count(const Container& container, const T& val)
    -> typename std::iterator_traits<decltype(std::begin(container))>::difference_type
{
    return
        std::count(
            std::begin(container),
            std::end(container),
            val);
}


template<typename Container, typename UnaryPredicate>
auto count_if(const Container& container, UnaryPredicate&& pred)
    -> typename std::iterator_traits<decltype(std::begin(container))>::difference_type
{
    return
        std::count_if(
            std::begin(container),
            std::end(container),
            std::forward<UnaryPredicate>(pred));
}


template<typename Container, typename UnaryPredicate>
auto all(const Container& container, UnaryPredicate&& pred) -> bool
{
    return
        std::all_of(
            std::begin(container),
            std::end(container),
            std::forward<UnaryPredicate>(pred));
}


template<typename Container, typename UnaryPredicate>
auto any(const Container& container, UnaryPredicate&& pred) -> bool
{
    return
        std::any_of(
            std::begin(container),
            std::end(container),
            std::forward<UnaryPredicate>(pred));
}


template<typename Container>
auto is_sorted(const Container& container) -> bool
{
    return
        std::is_sorted(
            std::begin(container),
            std::end(container));
}


template<typename Container, typename Compare>
auto is_sorted(const Container& container, Compare&& comp) -> bool
{
    return
        std::is_sorted(
            std::begin(container),
            std::end(container),
            std::forward<Compare>(comp));
}


template<typename Container, typename T>
auto reduce(const Container& container, T&& init) -> T
{
    for (const auto& elem : container)
        init += elem;
    return init;
}


template<typename Container, typename T, typename BinaryOperation>
auto reduce(const Container& container, T&& init, BinaryOperation&& op) -> T
{
    for (const auto& elem : container)
        init = op(init, elem);
    return init;
}


template<class T>
auto to_string(const T& value) ->
    decltype(std::to_string(value))
{
    return std::to_string(value);
}


template<class T>
auto to_string(const T& value) ->
    typename std::enable_if<
        !std::is_scalar<T>::value,
        decltype(std::declval<std::ostream&>() << value, std::string{})
        >::type
{
    std::ostringstream out;
    out << value;
    return out.str();
}


template<class T>
auto to_string(const T& value) ->
    typename std::enable_if<
        mp_hasfn_with_signature(T, to_string, std::string(void)),
        std::string
        >::type
{
    return value.to_string();
}


template<class T>
auto to_string(const T& value) ->
    typename std::enable_if<
        mp_hasfn_with_signature(T, toString, std::string(void)),
        std::string
        >::type
{
    return value.toString();
}


template<template<class...> class T, class N, class... L>
auto join(const T<N, L...>& list, const std::string& delimiter = ",") ->
    decltype(to_string(*std::begin(list)), std::string{})
{
    std::string result;
    for (const auto& elem : list) {
        if (not result.empty()) {
            result += delimiter;
        }
        result += to_string(elem);
    }
    return result;
}


template<class T>
auto join(const T& list) ->
    typename std::decay<decltype(*std::begin(list))>::type
{
    typename std::decay<decltype(*std::begin(list))>::type result;
    for (const auto& elem : list) {
        result = combine(result, elem);
    }
    return result;
}


template<class T>
auto is_number(const T& s) -> bool
{
    auto it = s.begin();
    while (it != s.end() and std::isdigit(*it)) ++it;
    return not s.empty() and it == s.end();
}


template<class T>
std::vector<T> split(const T& text, const T& delims = " ")
{
    std::vector<T> tokens;

    auto start = text.find_first_not_of(delims);
    decltype(start) end = 0;

    while ((end = text.find_first_of(delims, start)) != T::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = text.find_first_not_of(delims, end);
    }

    if (start != T::npos) {
        tokens.push_back(text.substr(start));
    }

    return tokens;
}


template<std::size_t N, std::size_t M>
std::vector<std::string> split(const char(&s)[N], const char(&s2)[M] = " ")
{
    return split(std::string{s}, std::string{s2});
}


template<class T>
auto ltrim(const T& s) -> std::string
{
    const auto wsfront=std::find_if_not(s.begin(),s.end(), ::isspace);
    const auto wsback=std::end(s);
    return wsback <= wsfront ? std::string{} : std::string{wsfront,wsback};
}


template<std::size_t N>
auto ltrim(const char(&s)[N]) -> std::string
{
    return ltrim(std::string{s});
}


template<class T>
auto rtrim(const T& s) -> std::string
{
    const auto wsfront=std::begin(s);
    const auto wsback=std::find_if_not(s.rbegin(),s.rend(), ::isspace).base();
    return wsback <= wsfront ? T{} : T{wsfront,wsback};
}


template<std::size_t N>
auto rtrim(const char(&s)[N]) -> std::string
{
    return rtrim(std::string{s});
}

template<class T>
auto trim(const T& s) -> std::string
{
    const auto wsfront=std::find_if_not(s.cbegin(),s.cend(), ::isspace);
    const auto wsback=std::find_if_not(s.crbegin(),s.crend(), ::isspace).base();
    return wsback <= wsfront ? T{} : T{wsfront,wsback};
}


template<std::size_t N>
auto trim(const char(&s)[N]) -> std::string
{
    return trim(std::string{s});
}


template<class T>
auto to_lower(const T& s) -> decltype(T().begin(), T())
{
    T result(s);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}


template<std::size_t N>
auto to_lower(const char(&s)[N]) -> std::string
{
    return to_lower(std::string{s});
}


template<class T>
auto to_upper(const T& s) -> decltype(T().begin(), T())
{
    T result(s);
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}


template<std::size_t N>
auto to_upper(const char(&s)[N]) -> std::string
{
    return to_upper(std::string{s});
}


template<class T>
bool contains(const T& where, const T& what) {
    return where.find(what) != T::npos;
}


template<std::size_t N, std::size_t M>
auto contains(const char (&s)[N], const char (&s2)[M]) -> bool
{
    return contains(std::string{s}, std::string{s2});
}


template<class T>
bool starts_with(const T& where, const T& what) {
    return where.find(what) == 0;
}


template<std::size_t N, std::size_t M>
auto starts_with(const char (&s)[N], const char (&s2)[M]) -> bool
{
    return starts_with(std::string{s}, std::string{s2});
}


template<class T>
bool ends_with(const T& where, const T& what) {
    return where.size() - 1 - where.find_last_of(what) == 0;
}


template<std::size_t N, std::size_t M>
auto ends_with(const char (&s)[N], const char (&s2)[M]) -> bool
{
    return ends_with(std::string{s}, std::string{s2});
}


template<class T>
auto capitalize(T&& s) ->
    typename std::enable_if<
        mp_hasfn_with_signature(T, at, const char&(std::size_t)) ||
        mp_hasfn_with_signature(T, at, char&(std::size_t)),
        T
    >::type
{
    auto result = to_lower(std::forward<T>(s));
    if (not result.empty()) {
        result[0] = ::toupper(result.at(0));
    }
    return result;
}


template<std::size_t N>
auto capitalize(const char(&s)[N]) -> std::string
{
    return capitalize(std::string{s});
}


template<class T>
auto take(const T& list, const std::size_t N) -> T
{
    if (N >= list.size()) {
        return list;
    }

    T result;
    reserve(result, N);

    for (std::size_t i = 0; i < N; ++i) {
        nocopy::push_back(result, list[i]);
    }

    return result;
}


template<class T, class Fn>
auto take_while(const T& list, Fn fn) -> T
{
    T result;

    for (const auto& elem : list) {
        if (not fn(elem)) {
            break;
        }

        nocopy::push_back(result, elem);
    }

    return result;
}


template<class T>
auto drop(const T& list, const std::size_t N) -> T
{
    if (N >= list.size()) {
        return T{};
    }

    T result;
    reserve(result, list.size() - N);

    for (std::size_t i = N; i < list.size(); ++i) {
        nocopy::push_back(result, list[i]);
    }

    return result;
}


template<class T, class Fn>
auto drop_while(const T& list, Fn fn) -> T
{
    T result;

    std::size_t i = 0;

    for (; i < list.size(); ++i) {
        if (not fn(list[i])) {
            break;
        }
    }

    for (; i < list.size(); ++i) {
        nocopy::push_back(result, list[i]);
    }

    return result;
}


template<class T>
auto slice(const T& list) -> T
{
    return list;
}


template<class T>
auto slice(const T& list, std::int64_t i) -> T
{
    if (list.empty()) {
        return {};
    }

    if (i < 0) {
        if (static_cast<std::size_t>(-i) >= list.size()) {
            i = 0;
        }
        else {
            i = list.size() - -i;
        }
    }

    auto it = std::begin(list);
    std::advance(it, i);
    return T{it, std::end(list)};
}


template<class T>
auto slice(const T& list, std::int64_t i, std::int64_t j) -> T
{
    if (list.empty()) {
        return {};
    }

    if (i < 0) {
        if (static_cast<std::size_t>(-i) >= list.size()) {
            i = 0;
        }
        else {
            i = list.size() - -i;
        }
    }
    else {
        if (static_cast<std::size_t>(i) >= list.size()) {
            return {};
        }
    }

    if (j < 0) {
        if (static_cast<std::size_t>(-j) >= list.size()) {
            return {};
        }
        else {
            j = list.size() - -j;
        }
    }
    else {
        if (static_cast<std::size_t>(j) >= list.size()) {
            j = list.size();
        }
    }

    if (i > j) {
        return {};
    }

    auto it = std::begin(list);
    std::advance(it, i);

    auto it2 = std::begin(list);
    std::advance(it2, j);

    return T{it, it2};
}


template<class T>
auto front(const T& list) -> ContainerElemT<T>&
{
    if (not list.empty()) {
        return const_cast<ContainerElemT<T>&>(*std::begin(list));
    }

    throw std::runtime_error("Container is empty.");
}


template<class T>
auto back(const T& list) -> ContainerElemT<T>&
{
    if (not list.empty()) {
        return const_cast<ContainerElemT<T>&>(*list.rbegin());
    }

    throw std::runtime_error("Container is empty.");
}


template<class T>
auto tail(const T& list) -> T
{
    T result {list};
    nocopy::remove_front(result);
    return result;
}


template<class T>
auto empty(const T& list) -> bool
{
    return list.empty();
}


} // namespace utils


struct Dar {

};

std::ostream& operator << (std::ostream& out, const Dar& dar) {
    out << "Dar";
    return out;
}

namespace std {
std::string to_string(const Dar& dar) {
    return "Dar";
}
} // namespace std


int main() {
    using namespace std;

    const vector<int> v1 = {1,2,3,4,5,1,3,9,0,1,5};
    const set<int> s1 = {1,2,3,4,5,1,3,9,0,1,5};

    assert((utils::sort(v1) ==
            vector<int>{0,1,1,1,2,3,3,4,5,5,9}));

    assert((utils::sort(vector<int>{1,23,4}) ==
            vector<int>{1,4,23}));

    assert((utils::sort([](const int& i, const int& j){return i>j;},
                        vector<int>{1,23,4}) ==
            vector<int>{23,4,1}));

    assert((utils::filter([](const int i){return i == 1 ? true : false;},
                          vector<int>{1,2,3}) ==
            vector<int>{1}));

    assert((utils::filter([](const int i){return i == 1 ? true : false;},
                          s1) ==
            set<int>{1}));

    {
        std::vector<int> t {1,2,3,4,5,6};
        utils::nocopy::filter([](const int i){return i == 1 ? true : false;}, t);
        assert((t == std::vector<int>{1}));
    }

    {
        std::vector<int> t {1,2,3};
        utils::nocopy::map([](const int i){return i*i;}, t);
        assert((t == std::vector<int>{1,4,9}));
    }

    // from vector<int> to vector<double> smart casting by lambda return type
    assert((utils::map([](const int&i){return(double)i*i;},
                       v1) ==
            vector<double>{1,4,9,16,25,1,9,81,0,1,25}));

    // from set<int> to set<double> smart casting by lambda return type
    assert((utils::map([](const int& i){return (double)i*i;},
                       s1) ==
            set<double>{0,1,4,9,16,25,81}));

    assert((utils::combine(v1, v1) ==
            vector<int>{1,2,3,4,5,1,3,9,0,1,5,1,2,3,4,5,1,3,9,0,1,5}));

    assert((utils::combine(v1, s1) ==
            vector<int>{1,2,3,4,5,1,3,9,0,1,5,0,1,2,3,4,5,9}));

    assert((utils::combine(set<int>(), v1, s1) ==
            set<int>{0,1,2,3,4,5,9}));

    assert((utils::combine(vector<int>{1,2}, 4, set<int>{8,9}) ==
            vector<int>{1,2,4,8,9}));

    assert(utils::combine(v1) ==
           v1);

    assert(utils::contains(v1, 1));
    assert(utils::contains(s1, 1));
    assert(utils::find(v1, 9) != nullptr);
    assert(utils::find_if([](const int& i){return i > 3 ? true : false;}, v1) != nullptr);
    assert(utils::find_if([](const int& i){return i > 300 ? true : false;}, v1) == nullptr);
    assert(!utils::empty(v1));
    assert(utils::front(vector<int>{1,2,3}) == 1);
    assert(utils::front(set<int>{1,2,3}) == 1);
    assert(utils::back(vector<int>{1,2,3}) == 3);
    assert(utils::back(set<int>{1,2,3}) == 3);
    assert(utils::get(v1, 9) == 9);
    assert(utils::nth(v1, 4) == 5);
    assert(utils::is_sorted(vector<int>{1,2,3}));
    assert(not utils::is_sorted(vector<int>{3,1,2,3}));

    assert((utils::push_front(vector<int>{1,2,3}, 4) == vector<int>{4,1,2,3}));
    assert((utils::push_front(set<int>{1,2,3}, 4) == set<int>{1,2,3,4}));

    assert((utils::push_back(vector<int>{1,2,3}, 4) == vector<int>{1,2,3,4}));
    assert((utils::push_back(set<int>{1,2,3}, 4) == set<int>{1,2,3,4}));

    assert((utils::insert(vector<int>{1,2,3}, 0, 111) == vector<int>{111,1,2,3}));
    assert((utils::insert(vector<int>{1,2,3}, 1, 111) == vector<int>{1,111,2,3}));
    assert((utils::insert(vector<int>{1,2,3}, 2, 111) == vector<int>{1,2,111,3}));
    assert((utils::insert(vector<int>{1,2,3}, 3, 111) == vector<int>{1,2,3,111}));
    assert((utils::insert(vector<int>{1,2,3}, 4, 111) == vector<int>{1,2,3,111}));

    assert((utils::insert(set<int>{1,2,3}, 4, 111) == set<int>{1,2,3,111}));
    assert((utils::insert(set<int>{1,2,3}, 4, 111) == set<int>{1,111,2,3}));

    assert((utils::remove_n(vector<int>{1,2,3}, 0) == vector<int>{2,3}));
    assert((utils::remove_n(vector<int>{1,2,3}, 1) == vector<int>{1,3}));
    assert((utils::remove_n(vector<int>{1,2,3}, 2) == vector<int>{1,2}));
    assert((utils::remove_n(vector<int>{1,2,3}, 3) == vector<int>{1,2,3}));
    assert((utils::remove_n(set<int>{1,2,3}, 2) == set<int>{1,2}));

    assert((utils::remove(vector<int>{1,2,3}, 2) == vector<int>{1,3}));
    assert((utils::remove(set<int>{1,2,3}, 1) == set<int>{2,3}));

    assert(utils::to_string(1) == "1");

    struct Foo { std::string to_string() const { return "Foo"; } };
    struct Bar { std::string toString() const { return "Bar"; } };

    assert(utils::to_string(Foo{}) == "Foo");
    assert(utils::to_string(Bar{}) == "Bar");
    assert(utils::to_string(Dar{}) == "Dar");

    const vector<vector<int>> vv = {{ {1,2,3}, {4,5,6} } };

    assert(utils::join(vector<int>{1,2,3}) == "1,2,3");
    assert(utils::join(utils::join(vv)) == "1,2,3,4,5,6");
    assert(utils::join(utils::split("a,b,c", ","), " ") == "a b c");

    assert(utils::trim("   asd  ") == "asd");
    assert(utils::trim("   asd") == "asd");
    assert(utils::trim("asd  ") == "asd");
    assert(utils::trim("asd") == "asd");
    assert(utils::ltrim("  asd  ") == "asd  ");
    assert(utils::ltrim("asd  ") == "asd  ");
    assert(utils::rtrim("  asd  ") == "  asd");
    assert(utils::rtrim("asd  ") == "asd");

    assert(utils::combine(std::string("asd "), std::string("fgh")) == "asd fgh");
    assert(utils::combine(std::string("asd "), "fgh") == "asd fgh");
    assert(utils::combine("asd ", "fgh") == "asd fgh");

    assert(utils::to_lower("aSdDUnnbD") == "asddunnbd");
    assert(utils::to_upper("aSdDUnnbD") == "ASDDUNNBD");

    assert(utils::contains("aSdDUnnbD", "nnb"));
    assert(utils::starts_with("aSdDUnnbD", "aSd"));
    assert(utils::ends_with("aSdDUnnbDx", "nbDx"));

    assert(utils::capitalize("aSdDUnnbDx") == "Asddunnbdx");
    assert(utils::capitalize(std::string{"aSdDUnnbDx"}) == "Asddunnbdx");

    utils::nocopy::foreach([](const int& i){}, v1);

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::take(t, 3);
        assert((t == vector<int>{1,2,3}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::take_while(t, [](const int& i){return i<=3;});
        assert((t == vector<int>{1,2,3}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::drop(t, 3);
        assert((t == vector<int>{5,6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::drop_while(t, [](const int& i){return i<=3;});
        assert((t == vector<int>{5,6}));
    }

    assert((utils::take(vector<int>{1,2,3,5,6}, 3) == vector<int>{1,2,3}));
    assert((utils::take_while(vector<int>{1,2,3,5,6}, [](const int& i){return i<=3;}) ==
            vector<int>{1,2,3}));
    assert((utils::take_while(vector<int>{1,2,3,5,6}, [](const int& i){return i>3;}) ==
            vector<int>{}));

    assert((utils::drop(vector<int>{1,2,3,5,6}, 3) == vector<int>{5,6}));
    assert((utils::drop_while(vector<int>{1,2,3,5,6}, [](const int& i){return i<=3;}) ==
            vector<int>{5,6}));
    assert((utils::drop_while(vector<int>{1,2,3,5,6}, [](const int& i){return i>3;}) ==
            vector<int>{1,2,3,5,6}));

    assert((utils::slice(vector<int>{1,2,3,5,6}) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, 0) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, 3) == vector<int>{5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3) == vector<int>{3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -1) == vector<int>{6}));
    assert((utils::slice(set<int>{1,2,3,5,6}, -1) == set<int>{6}));

    assert((utils::slice(vector<int>{1,2,3,5,6}, 2, 3) == vector<int>{3}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 3) == vector<int>{3}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 4) == vector<int>{3,5}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 5) == vector<int>{3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, 6) == vector<int>{3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -4) == vector<int>{}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -3) == vector<int>{}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -2) == vector<int>{3}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -3, -1) == vector<int>{3,5}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -5, -1) == vector<int>{1,2,3,5}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -5) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -10) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{1,2,3,5,6}, -10, 50) == vector<int>{1,2,3,5,6}));
    assert((utils::slice(vector<int>{}, -5) == vector<int>{}));
    assert((utils::slice(set<int>{1,2,3,5,6}, -4, -1) == set<int>{2,3,5}));

    assert((utils::tail(vector<int>{1,2,3,4}) == vector<int>{2,3,4}));

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::tail(t);
        assert((t == vector<int>{2,3,5,6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 5);
        assert((t == vector<int>{}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 4);
        assert((t == vector<int>{6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 3);
        assert((t == vector<int>{5,6}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, 2, 3);
        assert((t == vector<int>{3}));
    }

    {
        vector<int> t{1,2,3,5,6};
        utils::nocopy::slice(t, -3, -1);
        assert((t == vector<int>{3,5}));
    }

    return 0;
}

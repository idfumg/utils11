#ifndef UTILS_HPP
#define UTILS_HPP


#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iostream>


namespace utils {


template<class...> struct voider { using type = void; };
template<class... T> using void_t = typename voider<T...>::type;
// template<class...> using void_t = void;


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
    struct mp_hasfunction_ : std::false_type {};                        \
                                                                        \
    template<class T>                                                   \
    struct mp_hasfunction_<T,                                           \
                           void_t<                                      \
                              decltype(                                 \
                                  std::declval<T&>().FNNAME(std::declval<Args>()...))>> \
        : std::true_type {};                                                     \
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


mp_hasfn_with_params_declare(push_back);
mp_hasfn_with_params_declare(push_front);
mp_hasfn_with_params_declare(insert);
mp_hasfn_with_params_declare(append);
mp_hasfn_with_params_declare(erase);
mp_hasfn_with_params_declare(pop_front);
mp_hasfn_with_params_declare(pop_back);
mp_hasfn_with_params_declare(reserve);
mp_hasfn_with_params_declare(at);
mp_hasfn_with_signature_declare(to_string);
mp_hasfn_with_signature_declare(toString);
mp_hasfn_with_signature_declare(at);
mp_hasfn_with_signature_declare(data);
mp_hasfn_with_params_declare(begin);


template<class T>
using ContainerElemT =
    typename std::decay<decltype(*std::begin(std::declval<T&>()))>::type;


namespace nocopy {


/*******************************************************************************
 * push_back
 *******************************************************************************/


template<class T, class V>
auto push_back_inner(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, push_back, ContainerElemT<T>) &&
        std::is_same<ContainerElemT<T>, typename std::decay<V>::type>::value,
        void
    >::type
{
    list.push_back(std::forward<V>(value));
}


template<class T, class V>
auto push_back_inner(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, insert, ContainerElemT<T>) &&
        std::is_same<ContainerElemT<T>, typename std::decay<V>::type>::value,
        void
    >::type
{
    list.insert(std::forward<V>(value));
}


auto push_back_inner(std::string& list, const std::string& value) -> void
{
    list.append(value);
}


template<class T, class V>
auto push_back_inner(T& list, const V& value) ->
    typename std::enable_if<
        std::is_same<ContainerElemT<T>, ContainerElemT<V>>::value &&
        !std::is_array<T>::value && !std::is_array<V>::value,
        void
    >::type
{
    for (auto&& e : value) {
        push_back_inner(list, e);
    }
}


template<class T>
auto push_back(T&) -> void
{

}


template<class T, class V, class... Args>
auto push_back(T& list, V&& value, Args&&... args) -> void
{
    push_back_inner(list, std::forward<V>(value));
    push_back(list, std::forward<Args>(args)...);
}


/*******************************************************************************
 * push_front
 *******************************************************************************/


template<class T, class V>
auto push_front_inner(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, push_front, V) &&
        std::is_same<ContainerElemT<T>, typename std::decay<V>::type>::value,
        void
    >::type
{
    list.push_front(std::forward<V>(value));
}


template<class T, class V>
auto push_front_inner(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params_insert<T, decltype(std::begin(list)), ContainerElemT<T>>() &&
        std::is_same<ContainerElemT<T>, typename std::decay<V>::type>::value,
        void
    >::type
{
    list.insert(std::begin(list), std::forward<V>(value));
}


auto push_front_inner(std::string& list, const std::string& value) -> void
{
    list.insert(0, value);
}

template<class T, class V>
auto push_front_inner(T& list, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params_insert<T, std::size_t, ContainerElemT<T>>() &&
        std::is_same<ContainerElemT<T>, typename std::decay<V>::type>::value,
        void
    >::type
{
    list.insert(0, std::forward<V>(value));
}


template<class T, class V>
auto push_front_inner(T& list, const V& value) ->
    typename std::enable_if<
        std::is_same<ContainerElemT<T>, ContainerElemT<V>>::value &&
        !std::is_array<T>::value && !std::is_array<V>::value,
        void
    >::type
{
    for (auto&& e : value) {
        push_front_inner(list, e);
    }
}


template<class T>
auto push_front(T&) -> void
{

}


template<class T, class V, class... Args>
auto push_front(T& list, V&& value, Args&&... args) -> void
{
    push_front_inner(list, std::forward<V>(value));
    push_front(list, std::forward<Args>(args)...);
}


/*******************************************************************************
 * insert
 *******************************************************************************/


template<class T, class V>
auto insert_inner(T& list, const std::size_t i, V&& value) ->
    typename std::enable_if<
        mp_hasfn_with_params_insert<
            T,
            decltype(std::begin(list)),
            typename std::decay<V>::type>() &&
        !std::is_array<V>::value,
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


template<class T, class V>
auto insert_inner(T& list, std::size_t i, const V& value) ->
    typename std::enable_if<
        mp_hasfn_with_params_insert<
            T,
            decltype(std::begin(list)),
            ContainerElemT<V>>() &&
        !std::is_array<V>::value,
        void
    >::type
{
    if (i >= list.size()) {
        for (auto&& e : value) {
            push_back(list, e);
        }
    }
    else {
        for (auto&& e : value) {
            auto it = std::begin(list);
            std::advance(it, std::min(i, list.size()));
            insert_inner(list, i, e);
            i++;
        }
    }
}


auto insert_inner(std::string& list, const std::size_t i, const std::string& value) ->
    void
{
    if (i == 0) {
        push_front(list, value);
    }
    else if (i >= list.size()) {
        push_back(list, value);
    }
    else {
        list.insert(i, value);
    }
}


template<class T>
auto insert(T& list, const std::size_t i) -> void
{

}


template<class T, class V, class... Args>
auto insert(T& list, const std::size_t i, V&& v, Args&&... args) -> void
{
    insert_inner(list, i, std::forward<V>(v));
    insert(list, i, std::forward<Args>(args)...);
}


/*******************************************************************************
 * pop_front
 *******************************************************************************/


template<class T>
auto pop_front(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, pop_front, void),
        void
    >::type
{
    if (list.empty()) {
        return;
    }

    list.pop_front();
}


template<class T>
auto pop_front(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    if (list.empty()) {
        return;
    }

    list.erase(std::begin(list));
}


/*******************************************************************************
 * pop_back
 *******************************************************************************/


template<class T>
auto pop_back(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, pop_back, void),
        void
    >::type
{
    if (list.empty()) {
        return;
    }

    list.pop_back();
}


template<class T>
auto pop_back(T& list) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    if (list.empty()) {
        return;
    }

    auto it = std::begin(list);
    std::advance(it, list.size() - 1);
    list.erase(it);
}


/*******************************************************************************
 * remove_n
 *******************************************************************************/


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


/*******************************************************************************
 * remove
 *******************************************************************************/


template<class T, class ValueT>
auto remove_(T& list, const ValueT& value) ->
    typename std::enable_if<
        !std::is_array<ValueT>::value &&
        std::is_same<ContainerElemT<T>, ValueT>::value,
        void
    >::type
{
    auto it = std::find(std::begin(list), std::end(list), value);
    if (it != std::end(list)) {
        list.erase(it);
    }
}


template<class T, class V>
auto remove_(T& list1, const V& list2) ->
    typename std::enable_if<
        !std::is_array<V>::value &&
        std::is_same<ContainerElemT<T>, ContainerElemT<V>>::value,
        void
    >::type
{
    for (const auto& e : list2) {
        remove_(list1, e);
    }
}


auto remove_(std::string& list, const std::string& value) -> void
{
    if (value.size() > list.size()) {
        return;
    }

    auto found = list.find(value);
    if (found != std::string::npos) {
        auto it1 = std::begin(list);
        auto it2 = std::begin(list);
        std::advance(it1, found);
        std::advance(it2, found + value.size());
        list.erase(it1, it2);
    }
}


template<class T>
auto remove(T&& value) -> void
{

}


template<class T, class V, class... Args>
auto remove(T& list, const V& value, Args&&... args) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, erase, decltype(std::begin(list))),
        void
    >::type
{
    remove_(list, value);
    remove(list, std::forward<Args>(args)...);
}


/*******************************************************************************
 * reserve
 *******************************************************************************/


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


/*******************************************************************************
 * filter
 *******************************************************************************/


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


/*******************************************************************************
 * sort
 *******************************************************************************/


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


/*******************************************************************************
 * map
 *******************************************************************************/


template<class Fn, class T>
auto map(Fn fn, T& list) -> void
{
    list.erase(
        std::transform(std::begin(list), std::end(list), std::begin(list), fn),
        std::end(list));
}


/*******************************************************************************
 * foreach
 *******************************************************************************/

template<class Fn, template<class...> class T, class... U>
auto foreach(Fn fn, const T<U...>& list) -> void
{
    for (const auto& elem : list) {
        fn(elem);
    }
}


/*******************************************************************************
 * take
 *******************************************************************************/


template<class T>
auto take(T& list, const std::size_t N) -> void
{
    list.erase(std::copy_n(
                   std::begin(list),
                   N,
                   std::begin(list)),
               std::end(list));
}


/*******************************************************************************
 * take_while
 *******************************************************************************/


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


/*******************************************************************************
 * drop
 *******************************************************************************/


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


/*******************************************************************************
 * drop_while
 *******************************************************************************/


template<class T, class Fn>
auto drop_while(T& list, Fn fn) -> void
{
    auto it = std::begin(list);

    while (it != std::end(list) and fn(*it)) {
        ++it;
    }

    list.erase(std::begin(list), it);
}


/*******************************************************************************
 * tail
 *******************************************************************************/


template<class T>
auto tail(T& list) -> void
{
    pop_front(list);
}


/*******************************************************************************
 * slice
 *******************************************************************************/


template<class T>
auto slice_inner(T& list, std::int64_t i) -> std::int64_t
{
    if (list.empty()) {
        return 0;
    }

    if (i < 0) {
        if (static_cast<std::size_t>(-i) >= list.size()) {
            i = 0;
        }
        else {
            i = list.size() - -i;
        }
    }

    return i;
}


template<class T>
auto slice_inner(const T& list, std::int64_t i, std::int64_t j) ->
    std::pair<std::int64_t, std::int64_t>
{
    if (list.empty()) {
        return {0, list.size()};
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

    return {i, j};
}


template<class T>
auto slice(T&) -> void
{

}


template<class T>
auto slice(T& list, const std::int64_t i) -> void
{
    auto it = std::begin(list);
    std::advance(it, nocopy::slice_inner(list, i));

    list.erase(std::begin(list), it);
}


template<class T>
auto slice(T& list, const std::int64_t i, const std::int64_t j) -> void
{
    const auto ij = slice_inner(list, i, j);

    auto it1 = std::begin(list);
    std::advance(it1, ij.first);
    list.erase(std::begin(list), it1);

    auto it2 = std::begin(list);
    std::advance(it2, ij.second - ij.first);
    list.erase(it2, std::end(list));
}


/*******************************************************************************
 * combine
 *******************************************************************************/


template<class T, class... Args>
auto combine(T&& list1, Args... args) -> void
{
    nocopy::push_back(std::forward<T>(list1), std::forward<Args>(args)...);
}


} // namespace nocopy


/*******************************************************************************
 * push_front
 *******************************************************************************/


template<class T, class... Args>
auto push_front(const T& list, Args&&... args) -> T
{
    T result{list};
    nocopy::push_front(result, std::forward<Args>(args)...);
    return result;
}


template<class... Args>
auto push_front(const std::string& list, Args&&... args) -> std::string
{
    std::string result{list};
    nocopy::push_front(result, std::forward<Args>(args)...);
    return result;
}


/*******************************************************************************
 * push_back
 *******************************************************************************/


template<class T, class... Args>
auto push_back(const T& list, Args&&... args) -> T
{
    T result{list};
    nocopy::push_back(result, std::forward<Args>(args)...);
    return result;
}


template<class... Args>
auto push_back(const std::string& list, Args&&... args) -> std::string
{
    std::string result{list};
    nocopy::push_back(result, std::forward<Args>(args)...);
    return result;
}


/*******************************************************************************
 * pop_back
 *******************************************************************************/


template<class T>
auto pop_back(const T& list) -> T
{
    T result{list};
    nocopy::pop_back(result);
    return result;
}


template<std::size_t N>
auto pop_back(const char(&s)[N]) -> std::string
{
    std::string result{s};
    nocopy::pop_back(result);
    return result;
}


/*******************************************************************************
 * pop_front
 *******************************************************************************/


template<class T>
auto pop_front(const T& list) -> T
{
    T result{list};
    nocopy::pop_front(result);
    return result;
}


template<std::size_t N>
auto pop_front(const char(&s)[N]) -> std::string
{
    std::string result{s};
    nocopy::pop_front(result);
    return result;
}


/*******************************************************************************
 * insert
 *******************************************************************************/


template<class T, class... Args>
auto insert(const T& param, const std::size_t i, Args&&... args) -> T
{
    T result{param};
    nocopy::insert(result, i, std::forward<Args>(args)...);
    return result;
}


template<class... Args>
auto insert(const std::string& param, const std::size_t i, Args&&... args) -> std::string
{
    std::string result{param};
    nocopy::insert(result, i, std::forward<Args>(args)...);
    return result;
}


/*******************************************************************************
 * remove_n
 *******************************************************************************/


template<class T, class V>
auto remove_n(const T& list, const V i) ->
    typename std::enable_if<
        !std::is_floating_point<V>::value,
        T
    >::type
{
    T result{list};
    nocopy::remove_n(result, i);
    return result;
}


template<std::size_t N, class V>
auto remove_n(const char(&s)[N], const V i) ->
    typename std::enable_if<
        !std::is_floating_point<V>::value,
        std::string
    >::type
{
    std::string result{s};
    nocopy::remove_n(result, i);
    return result;
}


/*******************************************************************************
 * remove
 *******************************************************************************/


template<class T, class V, class... Args>
auto remove(const T& list, const V& value, Args&&... args) -> T
{
    T result{list};
    nocopy::remove(result, value);
    nocopy::remove(result, std::forward<Args>(args)...);
    return result;
}


template<class V, class... Args>
auto remove(const std::string& list, const V& value, Args&&... args) -> std::string
{
    std::string result{list};
    nocopy::remove(result, value);
    nocopy::remove(result, std::forward<Args>(args)...);
    return result;
}


/*******************************************************************************
 * reserve
 *******************************************************************************/


template<class... Args>
auto reserve(Args&& ...args) -> void
{
    nocopy::reserve(std::forward<Args>(args)...);
}


/*******************************************************************************
 * filter
 *******************************************************************************/


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


/*******************************************************************************
 * sort
 *******************************************************************************/


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


/*******************************************************************************
 * convert
 *******************************************************************************/


template<class ResultT, class T>
auto convert(T&& list) -> ResultT
{
    return static_cast<ResultT>(std::forward<T>(list));
}


/*******************************************************************************
 * map
 *******************************************************************************/


template<class Fn, template<class...> class T, class... U>
auto map(Fn fn, const T<U...>& list) -> T<decltype(fn(*std::begin(list)))>
{
    T<decltype(fn(*std::begin(list)))> result;
    for (const auto& elem : list) {
        nocopy::push_back(result, fn(elem));
    }
    return result;
}


/*******************************************************************************
 * combine
 *******************************************************************************/


template<class T, class... Args>
auto combine(const T& list1, Args... args) -> T
{
    T result;
    nocopy::push_back(result, list1, std::forward<Args>(args)...);
    return result;
}


template<class... Args>
auto combine(const std::string& list1, Args... args) -> std::string
{
    std::string result{};
    nocopy::push_back(result, list1, std::forward<Args>(args)...);
    return result;
}


/*******************************************************************************
 * to_string
 *******************************************************************************/


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


template<template<class...> class T, class V, class... Args>
auto to_string(const T<V, Args...>& value) ->
    typename std::enable_if<
        std::is_same<T<V, Args...>, std::vector<V, Args...>>::value,
        std::string
    >::type
{
    std::string result;
    for (auto it = std::begin(value); it != std::end(value); ++it) {
        if (not result.empty()) {
            result += ",";
        }
        result += to_string(*it);
    }
    return "[" + result + "]";
}


template<template<class...> class T, class V, class... Args>
auto to_string(const T<V, Args...>& value) ->
    typename std::enable_if<
        std::is_same<T<V, Args...>, std::set<V, Args...>>::value,
        std::string
    >::type
{
    std::string result;
    for (auto it = std::begin(value); it != std::end(value); ++it) {
        if (not result.empty()) {
            result += ",";
        }
        result += to_string(*it);
    }
    return "{" + result + "}";
}


template<template<class...> class T, class V, class... Args>
auto to_string(const T<V, Args...>& value) ->
    typename std::enable_if<
        std::is_same<T<V, Args...>, std::map<V, Args...>>::value,
        std::string
    >::type
{
    std::string result;
    for (auto it = std::begin(value); it != std::end(value); ++it) {
        if (not result.empty()) {
            result += ",\n";
        }
        result += "  " + to_string(it->first) + ": " + to_string(it->second);
    }
    return "{\n" + result + "\n}";
}


template<template<class...> class T, class V, class... Args>
auto to_string(const T<V, Args...>& value) ->
    typename std::enable_if<
        std::is_same<T<V, Args...>, std::unordered_map<V, Args...>>::value,
        std::string
    >::type
{
    std::string result;
    for (auto it = std::begin(value); it != std::end(value); ++it) {
        if (not result.empty()) {
            result += ",\n";
        }
        result += "  " + to_string(it->first) + ": " + to_string(it->second);
    }
    return "#{\n" + result + "\n}";
}


/*******************************************************************************
 * contains
 *******************************************************************************/


template<template<class...> class T, class ValueT, class... L>
auto contains_(const T<ValueT, L...>& list, const ValueT& value) ->
    typename std::enable_if<
        decltype(*std::begin(list), bool{}){true},
        bool
    >::type
{
    return std::find(std::begin(list), std::end(list), value) != std::end(list);
}


auto contains_(const std::string& list, const std::string& value) -> bool
{
    return list.find(value) != std::string::npos;
}


template<class T, class U>
auto contains_(const T& list, const U& list2) ->
    typename std::enable_if<
        std::is_same<ContainerElemT<T>, ContainerElemT<U>>::value &&
        decltype(list2.empty(), bool{}){true} &&
        decltype(*std::begin(list), *std::begin(list2), bool{}){true},
        bool
    >::type
{
    if (list2.empty()) {
        return true;
    }

    for (auto it = std::begin(list); it != std::end(list); ++it) {
        if (static_cast<std::size_t>(
                std::abs(
                    std::distance(it, std::end(list)))) < list2.size()) {
            return false;
        }

        bool yes = true;
        auto itk = it;

        for (auto it2 = std::begin(list2); it2 != std::end(list2); ++itk, ++it2) {
            if (*itk != *it2) {
                yes = false;
                break;
            }
        }

        if (yes) {
            return true;
        }
    }
    return false;
}


template<std::size_t N, class V>
auto contains_(const char(&s)[N], V&& value) ->
    typename std::enable_if<
        std::is_same<V, char>::value,
        bool
    >::type
{
    for (const char& ch : s) {
        if (ch == value) {
            return true;
        }
    }
    return false;
}


template<class T>
auto contains(const T& list) -> bool
{
    return true;
}


template<class T, class U, class... Args>
auto contains(T&& list, U&& list2, Args&&... args) -> bool
{
    return
        contains_(std::forward<T>(list), std::forward<U>(list2)) and
        contains(std::forward<T>(list), std::forward<Args>(args)...);
}


template<class U, class... Args>
auto contains(const std::string& list, U&& list2, Args&&... args) -> bool
{
    return
        contains_(list, std::forward<U>(list2)) and
        contains(list, std::forward<Args>(args)...);
}


/*******************************************************************************
 * find
 *******************************************************************************/


template<class T, class ValueT>
auto find(T&& list, const ValueT& value) ->
    typename std::enable_if<
        std::is_same<ContainerElemT<T>, ValueT>::value,
        ValueT*
     >::type
{
    for (const auto& elem : list) {
        if (elem == value) {
            return const_cast<ValueT*>(&elem);
        }
    }
    return nullptr;
}


/*******************************************************************************
 * find_if
 *******************************************************************************/


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


/*******************************************************************************
 * get
 *******************************************************************************/


template<class T, class ValueT>
auto get(const T& list, const ValueT& value) ->
    typename std::enable_if<
        !std::is_array<T>::value &&
        std::is_same<ContainerElemT<T>, ValueT>::value,
        ValueT&
    >::type
{
    for (const auto& elem : list) {
        if (elem == value) {
            return *const_cast<ValueT*>(&elem);
        }
    }
    throw std::runtime_error(
        "Value does not exists in list. [" + to_string(value) + "]");
}


template<std::size_t N, class V>
auto get(const char(&s)[N], const V value) ->
    typename std::enable_if<
        sizeof(V) == sizeof(char),
        const char&
    >::type
{
    for (const char& c : s) {
        if (c == value) {
            return c;
        }
    }
    throw std::runtime_error(
        "No value found in literal string. [" + to_string(value) + "]");
}


/*******************************************************************************
 * nth
 *******************************************************************************/


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
    throw std::runtime_error(
        "nth: it is a wrong element position. [" + to_string(N) + "]");
}


/*******************************************************************************
 * get_if
 *******************************************************************************/


template<template<class...> class T, class ValueT, class... L, class FindFn>
auto get_if(FindFn fn, const T<ValueT, L...>& list) -> ValueT&
{
    for (const auto& elem : list) {
        if (fn(elem)) {
            return *const_cast<ValueT*>(&elem);
        }
    }
    throw std::runtime_error(
        "Value does not exists in the list.");
}


/*******************************************************************************
 * count
 *******************************************************************************/


template<class T, class ValueT>
auto count(const ValueT& val, const T& param) ->
    typename std::enable_if<
        decltype(std::begin(param), true){true},
        std::size_t
    >::type
{
    return
        std::count(
            std::begin(param),
            std::end(param),
            val);
}


template<class ValueT, class T>
auto count(const ValueT& value, const T& param) ->
    typename std::enable_if<
        decltype(value == param, true){true},
        std::size_t
    >::type
{
    return value == param ? 1 : 0;
}


template<class ValueT, class T, class... Args>
auto count(const ValueT& value, const T& param, Args&&... args) -> std::size_t
{
    return count(value, param) + count(value, std::forward<Args>(args)...);
}


/*******************************************************************************
 * count_if
 *******************************************************************************/


template<class T, class Fn>
auto count_if(Fn pred, const T& param) ->
    typename std::enable_if<
        decltype(std::begin(param), true){true},
        std::size_t
    >::type
{
    return
        std::count_if(
            std::begin(param),
            std::end(param),
            pred);
}


template<class Fn, class T>
auto count_if(const Fn& fn, const T& param) ->
    typename std::enable_if<
        decltype(fn(param), true){true},
        std::size_t
    >::type
{
    return fn(param) ? 1 : 0;
}


template<class Fn, class T, class... Args>
auto count_if(Fn fn, const T& param, Args&&... args) -> std::size_t
{
    return count_if(fn, param) + count_if(fn, std::forward<Args>(args)...);
}


/*******************************************************************************
 * all
 *******************************************************************************/


template<class T>
auto all(const T& param) ->
    typename std::enable_if<
        decltype(static_cast<bool>(param), true){true},
        bool
    >::type
{
    return param;
}


template<class T>
auto all(const T& param) ->
    typename std::enable_if<
        decltype(std::begin(param), true){true},
        bool
    >::type
{
    for (auto&& e : param) {
        if (not all(e)) {
            return false;
        }
    }
    return true;
}


template<class T, class... Args>
auto all(const T& param, Args&&... args) -> bool
{
    return all(param) and all(std::forward<Args>(args)...);
}


/*******************************************************************************
 * all_if
 *******************************************************************************/


template<class T, class Fn>
auto all_if(Fn pred, const T& param) ->
    typename std::enable_if<
        decltype(static_cast<bool>(param), true){true},
        bool
    >::type
{
    return param;
}


template<class T, class Fn>
auto all_if(Fn pred, const T& container) ->
    typename std::enable_if<
        decltype(std::begin(container), true){true},
        bool
    >::type
{
    return
        std::all_of(
            std::begin(container),
            std::end(container),
            std::forward<Fn>(pred));
}


template<class Fn, class T, class... Args>
auto all_if(Fn pred, const T& param, Args&&... args) -> bool
{
    return all_if(pred, param) and all_if(pred, std::forward<Args>(args)...);
}


/*******************************************************************************
 * any
 *******************************************************************************/


template<class T>
auto any(const T& param) ->
    typename std::enable_if<
        decltype(static_cast<bool>(param), true){true},
        bool
    >::type
{
    return param;
}


template<class T>
auto any(const T& param) ->
    typename std::enable_if<
        decltype(std::begin(param), true){true},
        bool
    >::type
{
    for (auto&& e : param) {
        if (any(e)) {
            return true;
        }
    }
    return false;
}


template<class T, class... Args>
auto any(const T& param, Args&&... args) -> bool
{
    return any(param) or any(std::forward<Args>(args)...);
}


/*******************************************************************************
 * any_if
 *******************************************************************************/


template<class T, class Fn>
auto any_if(Fn pred, const T& param) ->
    typename std::enable_if<
        decltype(static_cast<bool>(param), true){true},
        bool
    >::type
{
    return param;
}


template<class T, class Fn>
auto any_if(Fn pred, const T& param) ->
    typename std::enable_if<
        decltype(std::begin(param), true){true},
        bool
    >::type
{
    return
        std::any_of(
            std::begin(param),
            std::end(param),
            std::forward<Fn>(pred));
}


template<class Fn, class T, class... Args>
auto any_if(Fn pred, const T& param, Args&&... args) -> bool
{
    return any_if(pred, param) or any_if(pred, std::forward<Args>(args)...);
}


/*******************************************************************************
 * is_sorted
 *******************************************************************************/


template<class Container>
auto is_sorted(const Container& container) -> bool
{
    return
        std::is_sorted(
            std::begin(container),
            std::end(container));
}


template<class Container, class Compare>
auto is_sorted(const Container& container, Compare&& comp) -> bool
{
    return
        std::is_sorted(
            std::begin(container),
            std::end(container),
            std::forward<Compare>(comp));
}


template<std::size_t N>
auto is_sorted(const char(&s)[N]) -> bool
{
    if (N == 1 or N == 2) {
        return true;
    }

    return is_sorted(std::string{s});
}


/*******************************************************************************
 * reduce
 *******************************************************************************/


template<class Container, class T>
auto reduce(const Container& container, T&& init) -> T
{
    for (const auto& elem : container) {
        init += elem;
    }
    return init;
}


template<class Container, class T, class BinaryOperation>
auto reduce(const Container& container, T&& init, const BinaryOperation& op) -> T
{
    for (const auto& elem : container) {
        init = op(std::forward<T>(init), elem);
    }
    return init;
}


/*******************************************************************************
 * flatten
 *******************************************************************************/


template<class T>
auto flatten(const T& list) ->
    typename std::decay<decltype(*std::begin(list))>::type
{
    typename std::decay<decltype(*std::begin(list))>::type result;
    for (const auto& elem : list) {
        result = combine(result, elem);
    }
    return result;
}


/*******************************************************************************
 * join
 *******************************************************************************/


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


auto join(const std::string& s) -> std::string
{
    return s;
}


template<std::size_t N>
auto join(const char(&s)[N]) -> std::string
{
    return std::string{s};
}


/*******************************************************************************
 * is_number
 *******************************************************************************/


template<class T>
auto is_number(const T& s) -> bool
{
    auto it = s.begin();
    while (it != s.end() and std::isdigit(*it)) ++it;
    return not s.empty() and it == s.end();
}


/*******************************************************************************
 * split
 *******************************************************************************/


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


/*******************************************************************************
 * ltrim
 *******************************************************************************/


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


/*******************************************************************************
 * rtrim
 *******************************************************************************/


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


/*******************************************************************************
 * trim
 *******************************************************************************/


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


/*******************************************************************************
 * to_lower
 *******************************************************************************/


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


/*******************************************************************************
 * to_upper
 *******************************************************************************/


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


/*******************************************************************************
 * starts_with
 *******************************************************************************/


template<class T>
bool starts_with(const T& where, const T& what) {
    return where.find(what) == 0;
}


template<std::size_t N, std::size_t M>
auto starts_with(const char (&s)[N], const char (&s2)[M]) -> bool
{
    return starts_with(std::string{s}, std::string{s2});
}


/*******************************************************************************
 * ends_with
 *******************************************************************************/


template<class T>
bool ends_with(const T& where, const T& what) {
    return where.size() - 1 - where.find_last_of(what) == 0;
}


template<std::size_t N, std::size_t M>
auto ends_with(const char (&s)[N], const char (&s2)[M]) -> bool
{
    return ends_with(std::string{s}, std::string{s2});
}


/*******************************************************************************
 * capitalize
 *******************************************************************************/


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


/*******************************************************************************
 * take
 *******************************************************************************/


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


template<std::size_t N>
auto take(const char(&s)[N], const std::size_t i) -> std::string
{
    return take(std::string{s}, i);
}


/*******************************************************************************
 * take_while
 *******************************************************************************/


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


template<std::size_t N, class Fn>
auto take_while(const char(&s)[N], Fn&& fn) -> std::string
{
    return take_while(std::string{s}, std::forward<Fn>(fn));
}


/*******************************************************************************
 * drop
 *******************************************************************************/


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


template<std::size_t N>
auto drop(const char(&s)[N], const std::size_t i) -> std::string
{
    return drop(std::string{s}, i);
}


/*******************************************************************************
 * drop_while
 *******************************************************************************/


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


template<std::size_t N, class Fn>
auto drop_while(const char(&s)[N], Fn&& fn) -> std::string
{
    return drop_while(std::string{s}, std::forward<Fn>(fn));
}


/*******************************************************************************
 * slice
 *******************************************************************************/


template<class T>
auto slice(const T& list) -> T
{
    return list;
}


template<class T>
auto slice(const T& list, const std::int64_t i) -> T
{
    auto it = std::begin(list);
    std::advance(it, nocopy::slice_inner(list, i));

    return T{it, std::end(list)};
}


template<class T>
auto slice(const T& list, const std::int64_t i, const std::int64_t j) -> T
{
    const auto ij = nocopy::slice_inner(list, i, j);

    auto it1 = std::begin(list);
    std::advance(it1, ij.first);

    auto it2 = std::begin(list);
    std::advance(it2, ij.second);

    return T{it1, it2};
}


template<class T, class... Args>
auto slice(const T& list, Args&&... args) ->
    typename std::enable_if<
        std::is_array<T>::value,
        std::string
    >::type
{
    return slice(std::string{list}, std::forward<Args>(args)...);
}


/*******************************************************************************
 * front
 *******************************************************************************/


template<class T>
auto front(const T& list) -> ContainerElemT<T>&
{
    if (not list.empty()) {
        return const_cast<ContainerElemT<T>&>(*std::begin(list));
    }

    throw std::runtime_error("Container is empty.");
}


template<std::size_t N>
auto front(const char(&s)[N]) -> const char&
{
    return s[0];
}


/*******************************************************************************
 * back
 *******************************************************************************/


template<class T>
auto back(const T& list) -> ContainerElemT<T>&
{
    if (not list.empty()) {
        return const_cast<ContainerElemT<T>&>(*list.rbegin());
    }

    throw std::runtime_error("Container is empty.");
}


template<std::size_t N>
auto back(const char(&s)[N]) -> const char&
{
    return s[N - 2];
}


/*******************************************************************************
 * tail
 *******************************************************************************/


template<class T>
auto tail(const T& list) -> T
{
    T result {list};
    nocopy::pop_front(result);
    return result;
}


/*******************************************************************************
 * empty
 *******************************************************************************/


template<class T>
auto empty(const T& list) ->
    typename std::enable_if<
        decltype(list.size(), true){true},
        bool
    >::type
{
    return list.empty();
}


template<class T, class... Args>
auto empty(const T& param, Args&&... args) -> std::size_t
{
    return empty(param) + empty(std::forward<Args>(args)...);
}


/*******************************************************************************
 * size
 *******************************************************************************/


template<class T>
auto size(const T& param) ->
    typename std::enable_if<
        decltype(param.size(), true){true},
        std::size_t
    >::type
{
    return param.size();
}


template<class T, class... Args>
auto size(const T& param, Args&&... args) -> std::size_t
{
    return size(param) + size(std::forward<Args>(args)...);
}


/*******************************************************************************
 * max
 *******************************************************************************/


template<class T>
auto max(const T& param) -> std::size_t
{
    return param;
}


template<class T>
auto max(const T& param1, const T& param2) ->
    typename std::enable_if<
        decltype(std::max(param1, param2), true){true},
        std::size_t
    >::type
{
    return std::max(param1, param2);
}


template<class T, class... Args>
auto max(const T& param1, const T& param2, Args&&... args) -> std::size_t
{
    return std::max(max(param1, param2), max(std::forward<Args>(args)...));
}


/*******************************************************************************
 * min
 *******************************************************************************/


template<class T>
auto min(const T& param) -> T
{
    return param;
}


template<class T>
auto min(const T& param1, const T& param2) ->
    typename std::enable_if<
        decltype(std::min(param1, param2), true){true},
        T
    >::type
{
    return std::min(param1, param2);
}


template<class T, class... Args>
auto min(const T& param1, const T& param2, Args&&... args) -> T
{
    return std::min(min(param1, param2), min(std::forward<Args>(args)...));
}


/*******************************************************************************
 * std::map
 *******************************************************************************/


template<class T, class K>
auto get(const T& param, const K& key) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, at, typename T::key_type) &&
        !std::is_array<T>::value,
        typename std::decay<typename T::mapped_type>::type&
    >::type
{
    return const_cast<typename std::decay<typename T::mapped_type>::type&>(param.at(key));
}


template<class T, class K>
auto find(const T& param, const K& key) ->
    typename std::enable_if<
        mp_hasfn_with_params(T, at, typename T::key_type) &&
        !std::is_array<T>::value,
        typename std::decay<typename T::mapped_type>::type*
    >::type
{
    auto it = param.find(key);
    if (it == std::end(param)) {
        return nullptr;
    }
    return (typename std::decay<typename T::mapped_type>::type*)(&param.at(key));
}


template<class T>
auto keys(const T& param) ->
    typename std::enable_if<
        decltype(std::declval<typename T::key_type>(), true){true} &&
        !std::is_array<T>::value,
        std::vector<typename std::decay<typename T::key_type>::type>
    >::type
{
    std::vector<typename std::decay<typename T::key_type>::type> result;
    for (auto it = std::begin(param); it != std::end(param); ++it) {
        nocopy::push_back(result, it->first);
    }
    return result;
}

template<class T>
auto values(const T& param) ->
    typename std::enable_if<
        decltype(std::declval<typename T::mapped_type>(), true){true} &&
        !std::is_array<T>::value,
        std::vector<typename std::decay<typename T::mapped_type>::type>
    >::type
{
    std::vector<typename std::decay<typename T::mapped_type>::type> result;
    for (auto it = std::begin(param); it != std::end(param); ++it) {
        nocopy::push_back(result, it->second);
    }
    return result;
}


} // namespace utils


#endif // UTILS_HPP

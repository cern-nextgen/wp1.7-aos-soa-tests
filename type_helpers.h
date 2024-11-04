#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include <type_traits>


namespace __Type_Helpers {

constexpr std::size_t SOA_ALIGN = 16;

namespace detail {

struct UniversalType {
    template<typename T>
    operator T() {}
};

}  // namespace detail

template<typename T>
consteval auto MemberCounter(auto ...Members) {
    if constexpr (requires { T{ Members... }; } == false)
        return sizeof...(Members) - 1;
    else
        return MemberCounter<T>(Members..., detail::UniversalType{});
}

template <std::size_t M, typename T, typename S, typename Functor>
constexpr S apply_to_members(T& t, Functor&& f) {
    if constexpr (M == 0) {
        return {};
    } else if constexpr (M == 1) {
        auto& [m00] = t;
        return {f(m00)};
    } else if constexpr (M == 2) {
        auto& [m00, m01] = t;
        return {f(m00), f(m01)};
    } else if constexpr (M == 3) {
        auto& [m00, m01, m02] = t;
        return {f(m00), f(m01), f(m02)};
    } else if constexpr (M == 4) {
        auto& [m00, m01, m02, m03] = t;
        return {f(m00), f(m01), f(m02), f(m03)};
    } else if constexpr (M == 5) {
        auto& [m00, m01, m02, m03, m04] = t;
        return {f(m00), f(m01), f(m02), f(m03), f(m04)};
    } else if constexpr (M == 6) {
        auto& [m00, m01, m02, m03, m04, m05] = t;
        return {f(m00), f(m01), f(m02), f(m03), f(m04), f(m05)};
    } else if constexpr (M == 7) {
        auto& [m00, m01, m02, m03, m04, m05, m06] = t;
        return {f(m00), f(m01), f(m02), f(m03), f(m04), f(m05), f(m06)};
    }
}

template <std::size_t M, typename T, typename S, typename R> static S SOA_convert(R& val)
{
    auto id = [](auto& member) -> decltype(auto) { return member; };
    return apply_to_members<M, T, S>(reinterpret_cast<T&>(val), id);
}

template <std::size_t M, typename T, typename S, typename R> static S SOA_convert(R& val, std::size_t idx)
{
    auto id = [idx](auto& member) -> decltype(auto) { return member[idx]; };
    return apply_to_members<M, T, S>(reinterpret_cast<T&>(val), id);
}

template <typename T> requires std::is_standard_layout_v<T>
using plain_wrapper = T;

template <typename T>
using ref_wrapper = T&;

template <typename T>
using constref_wrapper = const T&;

template <std::size_t N>
struct soa_array {
    template <typename T>
    using soa_wrapper = /*alignas(__Type_Helpers::SOA_ALIGN)*/ T[N]; // TODO: Does this alignment work?
};

template <typename T>
using soaptr_wrapper = T*;

template <template <template <typename> typename> typename T, typename S>
struct AOS_arrayview
{
    AOS_arrayview(T<__Type_Helpers::plain_wrapper>* s, S T<__Type_Helpers::plain_wrapper>::*p) : source(s), pointer(p) {}
    S& operator[](std::size_t idx) { return source[idx].*pointer; }
    const S& operator[](std::size_t idx) const { return source[idx].*pointer; }

private:
    T<__Type_Helpers::plain_wrapper>* source;
    S T<__Type_Helpers::plain_wrapper>::*pointer;
};

template <template <template <typename> typename> typename S>
struct array_helper {
    template <typename T>
    using array_type_wrapper = AOS_arrayview<S, T>;
};

}  // namespace Type_Helpers

#endif  // TYPE_HELPERS_H
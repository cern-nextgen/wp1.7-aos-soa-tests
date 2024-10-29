#ifndef TYPE_HELPERS_H
#define TYPE_HELPERS_H

#include <type_traits>


namespace __Type_Helpers {

constexpr std::size_t SOA_ALIGN = 16;

template <typename T, typename S, typename R> static S SOA_convert(R& val)
{
    auto& [p1, p2, p3] = reinterpret_cast<T&>(val); // TODO: Need automatic counting, or define tuple with types, or reflection.
    return S(p1, p2, p3);
}

template <typename T, typename S, typename R> static S SOA_convert(R& val, std::size_t idx)
{
    auto& [p1, p2, p3] = reinterpret_cast<T&>(val); // TODO: Need automatic counting, or define tuple with types, or reflection.
    return S(p1[idx], p2[idx], p3[idx]);
}

template <typename T> requires std::is_standard_layout_v<T>
using plain_wrapper = T;

template <typename T>
using ref_wrapper = T&;

template <typename T>
using constref_wrapper = const T&;

template <std::size_t N> struct soa_array {
    template <typename T>
    using soa_wrapper = T[N]; // TODO: Does this alignment work? alignas(__Type_Helpers::SOA_ALIGN) T[N];
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
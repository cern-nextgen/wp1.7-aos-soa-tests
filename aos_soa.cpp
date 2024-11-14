#include "type_helpers.h"

#include <array>
#include <cstdlib>
#include <cstdio>
#include <tuple>
#include <utility>
#include <stdexcept>
#include <vector>

// Helper code

#define SOASTRUCT() template <template <typename> typename __SOA_T>
#define SOATYPE(SOA_TYPE) __SOA_T<SOA_TYPE>

// forward declarations
template <template <template <typename> typename> typename T> struct Type_Ref;
template <template <template <typename> typename> typename T> struct Type_ConstRef;
template <template <template <typename> typename> typename T> struct Type_Plain;
template <template <template <typename> typename> typename T, std::size_t N> struct Type_SOA; // array types, fixed size like std::array
template <template <template <typename> typename> typename T, std::size_t N> struct Type_AOS; //
template <template <template <typename> typename> typename T> struct Type_SOA_v; // vector types, variable size, owning, like std::vector
template <template <template <typename> typename> typename T> struct Type_AOS_v; //
template <template <template <typename> typename> typename T> struct Type_SOA_s; // span types, not owning
template <template <template <typename> typename> typename T> struct Type_AOS_s; // TODO: I assume for the span types, which are not owning, we'll need to differentiate between const and non const versions.
template <template <template <typename> typename> typename T> struct Type_SOAPtr; // TODO: Not sure if we really need this, but could be helpful to pass around to functions. Would be like a span, but doesn't know about the size

template <template <template <typename> typename> typename T>
struct Type_Ref : public T<__Type_Helpers::ref_wrapper>
{
    Type_Ref(const T<__Type_Helpers::ref_wrapper>& obj) : T<__Type_Helpers::ref_wrapper>(obj) {}
    Type_Ref(const Type_Ref<T>&) = default;
    Type_Ref() = default;

    auto get_ref(std::size_t = 0) { return *this; }
    auto get_ref(std::size_t = 0) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::ref_wrapper>, T<__Type_Helpers::constref_wrapper>>(*this); };
    auto get_copy(std::size_t = 0) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::ref_wrapper>, T<__Type_Helpers::plain_wrapper>>(*this); };
};

template <template <template <typename> typename> typename T>
struct Type_ConstRef : public T<__Type_Helpers::constref_wrapper>
{
    Type_ConstRef(const T<__Type_Helpers::ref_wrapper>& obj) : T<__Type_Helpers::constref_wrapper>(((const Type_Ref<T>)Type_Ref<T>(obj)).get_ref()) {}
    Type_ConstRef(const T<__Type_Helpers::constref_wrapper>& obj) : T<__Type_Helpers::constref_wrapper>(obj) {}
    Type_ConstRef(const Type_ConstRef<T>&) = default;
    Type_ConstRef() = default;

    auto get_ref(std::size_t = 0) const { return *this; }
    auto get_copy(std::size_t = 0) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::constref_wrapper>, T<__Type_Helpers::plain_wrapper>>(*this); };
};

template <template <template <typename> typename> typename T>
struct Type_SOAPtr : public T<__Type_Helpers::soaptr_wrapper>
{
    Type_SOAPtr(const T<__Type_Helpers::soaptr_wrapper>& obj) : T<__Type_Helpers::soaptr_wrapper>(obj) {}
    Type_SOAPtr(const Type_SOAPtr<T>&) = default;
    Type_SOAPtr() = default;
};

template <template <template <typename> typename> typename T>
struct Type_Plain : public T<__Type_Helpers::plain_wrapper>
{
    Type_Plain(const T<__Type_Helpers::plain_wrapper>& obj) : T<__Type_Helpers::plain_wrapper>(obj) {}
    Type_Plain(const Type_Plain<T>&) = default;
    Type_Plain() = default;

    auto get_ref(std::size_t = 0) { return __Type_Helpers::SOA_convert<T<__Type_Helpers::plain_wrapper>, T<__Type_Helpers::ref_wrapper>>(*this); };
    auto get_ref(std::size_t = 0) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::plain_wrapper>, T<__Type_Helpers::constref_wrapper>>(*this); };
    auto get_copy(std::size_t = 0) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::plain_wrapper>, T<__Type_Helpers::plain_wrapper>>(*this); };    
};

template <template <template <typename> typename> typename T, std::size_t N>
struct Type_SOA : public T<__Type_Helpers::soa_array<N>::template soa_wrapper>
{
    Type_Ref<T> operator[](std::size_t idx) { return get_ref(idx); }
    Type_ConstRef<T> operator[](std::size_t idx) const { return get_ref(idx); }

    auto get_ref(std::size_t idx) { return __Type_Helpers::SOA_convert<T<__Type_Helpers::soa_array<N>::template soa_wrapper>, T<__Type_Helpers::ref_wrapper>>(*this, idx); };
    auto get_ref(std::size_t idx) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::soa_array<N>::template soa_wrapper>, T<__Type_Helpers::constref_wrapper>>(*this, idx); };
    auto get_copy(std::size_t idx) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::soa_array<N>::template soa_wrapper>, T<__Type_Helpers::plain_wrapper>>(*this, idx); };
    auto get_ptrs() { return __Type_Helpers::SOA_convert<T<__Type_Helpers::soa_array<N>::template soa_wrapper>, T<__Type_Helpers::soaptr_wrapper>>(*this); };
};

template <template <template <typename> typename> typename T>
struct Type_SOA_v : public T<__Type_Helpers::template soaptr_wrapper> // TODO: Implement the same for Type_AOS_v
{
    Type_SOA_v(std::size_t v) : N(v) {} // TODO: implement constructor to allocate N elements
    Type_SOA_v(const Type_SOA_v&) = default;
    Type_Ref<T> operator[](std::size_t idx) { return get_ref(idx); }
    Type_ConstRef<T> operator[](std::size_t idx) const { return get_ref(idx); }

    auto get_ref(std::size_t idx) { return __Type_Helpers::SOA_convert<T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::ref_wrapper>>(*this, idx); };
    auto get_ref(std::size_t idx) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::constref_wrapper>>(*this, idx); };
    auto get_copy(std::size_t idx) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::plain_wrapper>>(*this, idx); };
    auto get_ptrs() { return __Type_Helpers::SOA_convert<T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::soaptr_wrapper>>(*this); };
private:
    std::size_t N;
};

template <template <template <typename> typename> typename T> // TODO: need a similar class for Type_SOA_s
struct Type_SOA_s : public T<__Type_Helpers::template soaptr_wrapper> // TODO: Implement the same for Type_AOS_s
{
    template <size_t M>
    Type_SOA_s(Type_SOA<T, M>& array, std::size_t offset = 0, std::size_t size = -1) {(void)array;(void)offset;(void)size;} // TODO: implement constructors to point to corresponding part of Type_SOA selected by offset/size (-1 = till the end)
    Type_SOA_s(Type_SOA_v<T>& vector, std::size_t offset = 0, std::size_t size = -1) {(void)vector;(void)offset;(void)size;} // Same for vector instead of array
    Type_SOA_s(const Type_SOA_s&) = default;
    Type_SOA_s(std::size_t size, void* buffer, bool placement_new = false, std::size_t bufferSize = -1) : N(size) { if (bufferSize != (std::size_t)-1 && N * sizeof(T<__Type_Helpers::plain_wrapper>) > bufferSize) throw std::runtime_error("buffer too small"); (void)buffer; if (placement_new) {}}
    Type_SOA_s(std::size_t size, void** buffer, bool placement_new = false, std::size_t* bufferSize = nullptr) : N(size) { if (bufferSize != nullptr) (void)buffer; if (placement_new) {}}
    Type_Ref<T> operator[](std::size_t idx) { return get_ref(idx); }
    Type_ConstRef<T> operator[](std::size_t idx) const { return get_ref(idx); }

    auto get_ref(std::size_t idx) { return __Type_Helpers::SOA_convert<T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::ref_wrapper>>(*this, idx); };
    auto get_ref(std::size_t idx) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::constref_wrapper>>(*this, idx); };
    auto get_copy(std::size_t idx) const { return __Type_Helpers::SOA_convert<const T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::plain_wrapper>>(*this, idx); };
    auto get_ptrs() { return __Type_Helpers::SOA_convert<T<__Type_Helpers::soaptr_wrapper>, T<__Type_Helpers::soaptr_wrapper>>(*this); };
private:
    std::size_t N;
};

template <template <template <typename> typename> typename T, std::size_t N>
struct Type_AOS : public T<__Type_Helpers::array_helper<T>::template array_type_wrapper>
{
    Type_AOS() : T<__Type_Helpers::array_helper<T>::template array_type_wrapper>(get_arrays()) {

    }
    auto& operator[](std::size_t idx) { return values[idx]; }
    const auto& operator[](std::size_t idx) const { return values[idx]; }

    auto get_ref(std::size_t idx) { return __Type_Helpers::SOA_convert<decltype(values[idx]), T<__Type_Helpers::ref_wrapper>>(values[idx]); };
    auto get_ref(std::size_t idx) const { return __Type_Helpers::SOA_convert<decltype(values[idx]), T<__Type_Helpers::constref_wrapper>>(values[idx]); };
    auto get_copy(std::size_t idx) const { return __Type_Helpers::SOA_convert<decltype(values[idx]), T<__Type_Helpers::plain_wrapper>>(values[idx]); };

    template <typename S>
    auto get_array_ptr(S T<__Type_Helpers::plain_wrapper>::*p) {
        return __Type_Helpers::AOS_arrayview<T, S>(values, p);
    }

private:
    T<__Type_Helpers::plain_wrapper> values[N];

    T<__Type_Helpers::array_helper<T>::template array_type_wrapper> get_arrays() {
        // T<Plain_Helper::type_wrapper>::auto* [p1, p2, p3] = values[0]; // TODO: Structured bindings do not work with member variable pointers, or I am too stupid to find out how
        return {this->get_array_ptr(&Type_Plain<T>::x), this->get_array_ptr(&Type_Plain<T>::y), this->get_array_ptr(&Type_Plain<T>::z)};
    }

};

// Helpers to automatically derive the right span class

// TODO: Right now this is just an empty class, can we make that become Type_AOS_s or Type_SOA_s depending on the constructor argument?
template <template <template <typename> typename> typename T, template <template <template <typename> typename> typename> typename S>
class Type_span {
public:
    template <typename...> struct span_class;
    template <> struct span_class<Type_AOS_v<T>> { using type = Type_AOS_s<T>; };
    template <> struct span_class<Type_SOA_v<T>> { using type = Type_SOA_s<T>; };

    Type_span(S<T>& ref, std::size_t offset = 0, std::size_t size = -1) : real_class(ref, offset, size) {}
    span_class<S<T>>::type real_class;
};

// TODO: In general, we need to overwrite the constructors, to either take a custom allocator function, or to create in place in existing memory like placement-new

// Type definition

struct sub_point // TODO: what do we do for nested SoAoS?
{
    int u, v;
};

SOASTRUCT() struct point_d
{
    SOATYPE(int) x; // TODO: What can we do for default values / constuctor?
    SOATYPE(float) y;
    SOATYPE(sub_point) z;
};
typedef Type_Plain<point_d> point;

// Usage

int main(int, char**)
{
    // Define some AOS / SOA vectors
    Type_SOA<point_d, 10> p_soa;
    Type_AOS<point_d, 10> p_aos;
    Type_SOA_v<point_d> p_soa_vector(10);
    // Type_AOS_v<point_d> p_aos_vector(10); // Todo: Type_AOS_v not yet implemented

    // Get some spans from the vectors
    Type_SOA_s<point_d> p_soa_span1(p_soa, 5, 3), p_soa_span2(p_soa_vector, 2, -1);
    Type_span autospan_from_soa(p_soa_vector); // Type_span should automatically derrive if soa/aos
    // Type_span<point_d, Type_AOS_v> autospan_from_aos(p_aos_v); // Todo: Type_AOS_v not yet implemented
    // Type_span<point_d, Type_AOS> autospan_from_aos_array_with_template_N(p_aos); // Todo: The same for array types with template parameter N, gets a bit tricky to deduce N automatically

    // Create objects from preallocated memory, or with allocators
    std::vector<char> buffer(1024 * 1024);
    Type_SOA_s<point_d> p_soa_s_placement(10 /*size of our vector*/, buffer.data() /*existing buiffer*/, true /*run placement new constructor*/, buffer.size() /*buffer size for checks, optional*/);
    std::array<void*, 10> buffers;
    Type_SOA_s<point_d> p_soa_s_placement2(10, buffers.data(), false, nullptr); // Here for individual buffers per member (only SOA, not AOS)

    // And some creation of vectors with allocation
    // Type_SOA_v<point_d, std::allocator<T>, continuous> p_soa_allocated1; // For vectors, we can provide an allocator
    // Type_SOA_v<point_d, std::allocator<T>, per_member> p_soa_allocated2;

    // Play around with getting copies of objects and references
    point p_plain;

    p_soa.x[1] = 10;
    p_aos.x[1] = 11;

    p_soa[2].x = 20;
    p_aos[2].x = 21;

    p_plain.x = 30;

    point tmp = p_aos[2];
    Type_Ref<point_d> tmp_ref = tmp.get_ref();
    const point tmp2 = tmp;
    [[maybe_unused]] Type_ConstRef<point_d> tmp_ref2 = tmp2.get_ref();
    [[maybe_unused]] Type_ConstRef<point_d> tmp_ref3 = tmp_ref;

    [[maybe_unused]] point x1 = p_plain.get_copy(), x2 = p_soa.get_copy(1), x3 = p_aos.get_copy(2), x4 = tmp_ref.get_copy();
    [[maybe_unused]] Type_SOAPtr<point_d> ptrs = p_soa.get_ptrs();

    auto test = p_aos.get_array_ptr(&point::z);
    test[3].u = 123;

    printf("FOO %3d %3d %3d - %3d %3d %3d - %3d\n", p_soa.x[1], p_aos[2].x, p_plain.x, p_aos.x[1], p_soa[2].x, tmp.x, p_aos[3].z.u);

    // Check references are working
    Type_Ref<point_d> r_plain = p_plain.get_ref();
    Type_Ref<point_d> r_soa = p_soa.get_ref(1);
    Type_Ref<point_d> r_aos = p_aos.get_ref(2);
    p_plain.x += 100;
    p_soa.x[1] += 100;
    p_aos[2].x += 100;
    printf("BAR %3d %3d %3d\n", r_soa.x, r_aos.x, r_plain.x);

    return 0;
}

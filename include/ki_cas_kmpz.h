#ifndef KI_CAS_KMPZ_H
#define KI_CAS_KMPZ_H

#include <cinttypes>
#include "ki_cas_test_hooks.h"
#include <gmp.h>
#include <flint/fmpz.h>
#include <intx/intx.hpp>

namespace KiCAS2 {

typedef intx::uint128 uint128_t;
typedef intx::uint256 uint256_t;
typedef intx::uint512 uint512_t;

std::to_chars_result to_chars(char* begin, char* end, uint128_t x) noexcept {
    if (x <= std::numeric_limits<uint64_t>::max())
        return std::to_chars(begin, end, uint64_t(x));

    /// The greatest power of 10 that fits into a 64-bit integer is 10^19.
    constexpr uint64_t exp10_19 = 10000000000000000000ull;
    constexpr int lower_max_digits = 19;

    // We can shift the result by 19 decimal digits to the right and convert
    // the quotient and remainder separately.
    // Note that even after dividing by 10^19, the quotient may not fit into
    // a 64-bit integer, so this function is recursive.
    const auto divrem_res = udivrem(x, exp10_19);
    const uint128_t high = divrem_res.quot;
    const uint64_t low = static_cast<uint64_t>(divrem_res.rem);

    auto upper_result = to_chars(begin, end, high);
    if(upper_result.ec != std::errc {}) return upper_result;

    auto lower_result = std::to_chars(upper_result.ptr, end, low);
    if(lower_result.ec != std::errc {}) return lower_result;
    auto lower_length = lower_result.ptr - upper_result.ptr;

    // The remainder is exactly 19 digits long, and we have to zero-pad
    // to the left if it is shorter.
    char* result_end = upper_result.ptr + lower_max_digits;
    char* result_begin = result_end - lower_length;
    std::memmove(result_begin, upper_result.ptr, size_t(lower_length));
    std::memset(upper_result.ptr, '0', size_t(lower_max_digits - lower_length));

    return { result_end, std::errc {} };
}

template<typename uintx_t, bool is_negative=false>
void mpz_init_set_x(mpz_t lhs, uintx_t rhs) {
    constexpr int num_bytes = sizeof(uintx_t);
    constexpr int num_bits = num_bytes*8;
    constexpr int num_limbs = num_bytes / sizeof(mp_limb_t);

    mpz_init2(lhs, num_bits);
    *reinterpret_cast<uintx_t*>(lhs->_mp_d) = rhs;

    if(sizeof(uintx_t) == sizeof(uint128_t) && sizeof(mp_limb_t) == sizeof(uint64_t)){
        const int size = num_limbs - (rhs <= std::numeric_limits<uint64_t>::max());
        lhs->_mp_size = is_negative ? (-size) : size;
    }else{
        lhs->_mp_size = intx::count_significant_words(rhs);
        if(is_negative) lhs->_mp_size += 1;
    }
}
template<bool is_negative=false> inline void mpz_init_set_uint128(mpz_t lhs, uint128_t rhs) {
    return mpz_init_set_x<uint128_t, is_negative>(lhs, rhs);
}
template<bool is_negative=false> inline void mpz_init_set_uint256(mpz_t lhs, uint256_t rhs) {
    return mpz_init_set_x<uint256_t, is_negative>(lhs, rhs);
}
template<bool is_negative=false> inline void mpz_init_set_uint512(mpz_t lhs, uint512_t rhs) {
    return mpz_init_set_x<uint512_t, is_negative>(lhs, rhs);
}

template<bool is_negative=false> fmpz u128_to_fmpz(uint128_t val) {
    fmpz out = 0;
    if(is_negative) fmpz_neg_uiui(&out, val[1], val[0]);
    else fmpz_set_uiui(&out, val[1], val[0]);
    return out;
}
template<bool is_negative=false> fmpz u256_to_fmpz(uint256_t val) {
    fmpz out = 0;
    fmpz_set_ui_array(
        &out,
        reinterpret_cast<const mp_limb_t*>(&val[0]),
        sizeof(uint256_t)/sizeof(mp_limb_t));
    if(is_negative) COEFF_TO_PTR(out)->_mp_size *= -1;
    return out;
}

template<typename uintx_t>
void write_uintx(std::string& str, uintx_t val) {
    str += intx::to_string(val);
}
inline void write_uint128(std::string& str, uint128_t val) { return write_uintx(str, val); }
inline void write_uint256(std::string& str, uint256_t val) { return write_uintx(str, val); }

}

#endif // KI_CAS_KMPZ_H

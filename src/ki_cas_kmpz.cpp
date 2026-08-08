#include "ki_cas_kmpz.h"

#include <charconv>

namespace KiCAS2 {

static std::to_chars_result to_chars(char *begin, char *end, uint128_t x) noexcept {
    if (x[1] == 0) return std::to_chars(begin, end, x[0]);

    /// The greatest power of 10 that fits into a 64-bit integer is 10^19.
    constexpr uint64_t exp10_19 = 10000000000000000000ull;
    constexpr int lower_max_digits = 19;
    char* const initial_begin = begin;

    while(x[1] != 0) {
        const auto divrem_res = udivrem(x, exp10_19);
        x = divrem_res.quot;
        const uint64_t rem = static_cast<uint64_t>(divrem_res.rem);

        const auto result = std::to_chars(begin, end, rem);
        assert(result.ec == std::errc());
        std::reverse(begin, result.ptr);
        std::memset(result.ptr, '0', size_t(begin + lower_max_digits - result.ptr));
        begin += lower_max_digits;
    }

    const auto result = std::to_chars(begin, end, x[0]);
    assert(result.ec == std::errc());
    std::reverse(begin, result.ptr);
    std::reverse(initial_begin, result.ptr);
    return result;
}

template<typename uintx_t, bool is_negative=false>
void mpz_init_set_x(mpz_t lhs, uintx_t rhs) {
    constexpr int num_bits = uintx_t::num_bits;
    constexpr int num_bytes = num_bits/8;
    constexpr int num_limbs = num_bytes / sizeof(mp_limb_t);

    mpz_init2(lhs, num_bits);
    *reinterpret_cast<uintx_t*>(lhs->_mp_d) = rhs;

    if(uintx_t::num_bits == 128 && sizeof(mp_limb_t) == sizeof(uint64_t)){
        const int size = num_limbs - (rhs <= std::numeric_limits<uint64_t>::max());
        lhs->_mp_size = is_negative ? (-size) : size;
    }else{
        if(sizeof(uint64_t) == sizeof(mp_limb_t)){
            lhs->_mp_size = intx::count_significant_words(rhs);
        }else{
            const auto bytes = intx::count_significant_bytes(rhs);
            lhs->_mp_size = (bytes + (sizeof(mp_limb_t)-1)) / sizeof(mp_limb_t);
        }
        if(is_negative) lhs->_mp_size += 1;
    }
}

template<bool is_negative> inline void mpz_init_set_uint128(mpz_t lhs, uint128_t rhs) {
    return mpz_init_set_x<uint128_t, is_negative>(lhs, rhs);
}
template void mpz_init_set_uint128<false>(mpz_t lhs, uint128_t rhs);
template void mpz_init_set_uint128<true>(mpz_t lhs, uint128_t rhs);

template<bool is_negative> inline void mpz_init_set_uint256(mpz_t lhs, uint256_t rhs) {
    return mpz_init_set_x<uint256_t, is_negative>(lhs, rhs);
}
template void mpz_init_set_uint256<false>(mpz_t lhs, uint256_t rhs);
template void mpz_init_set_uint256<true>(mpz_t lhs, uint256_t rhs);

template<bool is_negative> inline void mpz_init_set_uint512(mpz_t lhs, uint512_t rhs) {
    return mpz_init_set_x<uint512_t, is_negative>(lhs, rhs);
}
template void mpz_init_set_uint512<false>(mpz_t lhs, uint512_t rhs);
template void mpz_init_set_uint512<true>(mpz_t lhs, uint512_t rhs);

template<bool is_negative> fmpz u128_to_fmpz(uint128_t val) {
    fmpz out = 0;
    if(sizeof(mp_limb_t) == sizeof(uint64_t)){
        if(is_negative) fmpz_neg_uiui(&out, val[1], val[0]);
        else fmpz_set_uiui(&out, val[1], val[0]);
    }else{
        fmpz_set_ui_array(
            &out,
            reinterpret_cast<const mp_limb_t*>(&val[0]),
            16/sizeof(mp_limb_t));
        if(is_negative) COEFF_TO_PTR(out)->_mp_size *= -1;
    }
    return out;
}
template fmpz u128_to_fmpz<false>(uint128_t val);
template fmpz u128_to_fmpz<true>(uint128_t val);

template<bool is_negative> fmpz u256_to_fmpz(uint256_t val) {
    fmpz out = 0;
    fmpz_set_ui_array(
        &out,
        reinterpret_cast<const mp_limb_t*>(&val[0]),
        32/sizeof(mp_limb_t));
    if(is_negative) COEFF_TO_PTR(out)->_mp_size *= -1;
    return out;
}
template fmpz u256_to_fmpz<false>(uint256_t val);
template fmpz u256_to_fmpz<true>(uint256_t val);

template<typename uintx_t>
static void write_uintx(std::string& str, uintx_t val) {
    str += intx::to_string(val);
}

void write_uint128(std::string& str, uint128_t val) {
    constexpr size_t max_digits = std::numeric_limits<uint128_t>::digits10 + 1;
    char buffer[max_digits];
    const std::to_chars_result result = to_chars(buffer, buffer + max_digits, val);
    assert(result.ec == std::errc());
    str.append(buffer, result.ptr - buffer);
}

void write_uint256(std::string& str, uint256_t val) {
    write_uintx(str, val);
}

}  // namespace KiCAS2

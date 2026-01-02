#include "ki_cas_str_conversions.h"

#include <cassert>
#include <charconv>
#include "ki_cas_native_integer.h"
#include "ki_cas_native_rational.h"
#include <limits>

namespace KiCAS2 {

#ifndef NDEBUG
bool spoof_bignum_path = false;

bool& spoofBignumPath() {
    return spoof_bignum_path;
}

#define DEBUG_DISABLE_SPOOF spoof_bignum_path = false;
#else
static constexpr bool spoof_bignum_path = false;
#define DEBUG_DISABLE_SPOOF
#endif

constexpr size_t powers_of_ten[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
#if defined(__x86_64__) || defined(__aarch64__) || defined( _WIN64 )  // 64-bit
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
#endif
};

// Check this rather than specify it to make sure the macro worked
static_assert(sizeof(powers_of_ten)/sizeof(size_t) == std::numeric_limits<size_t>::digits10);

inline bool ckd_10_exponent(size_t& result, std::string_view str) noexcept {
#if defined(__x86_64__) || defined(__aarch64__) || defined( _WIN64 )  // 64-bit
    if(str.length() > 2){
        return true;
    }else if(str.length() == 1){
        result = powers_of_ten[str[0] - '0'];
        return false;
    }else{
        const size_t exp = str[0] * 10 + str[1];
        if(exp >= std::numeric_limits<size_t>::digits10) return true;
        result = powers_of_ten[exp];
        return false;
    }
#else  // 32-bit
    if(str.size() != 1) return true;
    const size_t exp = str[0] - '0';
    if(exp >= std::numeric_limits<size_t>::digits10) return true;
    result = powers_of_ten[exp];
    return false;
#endif
}

void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str) {
    const auto decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    return strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(f, str, decimal_index);
}

void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str, size_t decimal_index) {
    assert(str.at(decimal_index) == '.');
    assert(str.find('e') == std::string::npos);

    std::string_view lead = str.substr(0, decimal_index);

    // Omit trailing zeros
    size_t back_index = str.size()-1;
    while(str[back_index] == '0') back_index--;

    if(back_index == decimal_index){
        // Only trailing zeros, e.g. "2.0"
        fmpz_init_set_ui(fmpq_denref(f), 1);
        fmpz_init_set_strview(fmpq_numref(f), lead);
    }else{
        std::string_view trail = str.substr(decimal_index+1, back_index-decimal_index);
        return strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(f, lead, trail);
    }
}

void strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str_lead, std::string_view str_trail) {
    fmpz_t lead;
    fmpz_init_set_strview(lead, str_lead);
    fmpq_t tail;
    fmpz_init_set_strview(fmpq_numref(tail), str_trail);

    if(str_trail.length() >= std::numeric_limits<size_t>::digits10){
        _fmpz_promote(fmpq_denref(tail));
        flint_mpz_ui_pow_ui(COEFF_TO_PTR(fmpq_denref(tail)), 10, str_trail.size());
    }else{
        fmpz_init_set_ui(fmpq_denref(tail), powers_of_ten[str_trail.size()]);
    }
    fmpq_canonicalise(tail);

    fmpq_init(f);
    fmpq_add_fmpz(f, tail, lead);
    fmpq_clear(tail);
    fmpz_clear(lead);
}

void strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str) {
    const auto decimal_index = str.find('.');
    assert(decimal_index != std::string::npos);

    const auto exponent_index = str.find('e', decimal_index);
    assert(exponent_index != std::string::npos);

    return strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(f, str, decimal_index, exponent_index);
}

void strscientific2bigrat_NULL_TERMINATED__NOT_THREADSAFE(
    fmpq_t f, std::string_view str, size_t decimal_index, size_t e_index) {
    strdecimal2bigrat_NULL_TERMINATED__NOT_THREADSAFE(f, str.substr(0, e_index));

    e_index++;
    auto target = fmpq_numref(f);
    if(str[e_index] == '-'){
        e_index++;
        target = fmpq_denref(f);
    }

    fmpz_t copy;
    fmpz_init_set(copy, target);

    str = str.substr(e_index);
    size_t exp;
    if(ckd_10_exponent(exp, str) == false){
        fmpz_mul_ui(target, copy, exp);
    }else if(ckd_str2int(&exp, str) == false && !spoof_bignum_path){
        fmpz_t exponent;
        fmpz_init(exponent);
        fmpz_t ten;
        fmpz_init_set_ui(ten, 10);
        fmpz_pow_ui(exponent, ten, exp);
        fmpz_mul(target, copy, exponent);
        fmpz_clear(exponent);
    }else{
        DEBUG_DISABLE_SPOOF

        fmpz_t exponent;
        fmpz_init(exponent);
        fmpz_t exp;
        fmpz_init_set_strview(exp, str);
        fmpz_t ten;
        fmpz_init_set_ui(ten, 10);
        fmpz_pow_fmpz(exponent, ten, exp);
        fmpz_mul(target, copy, exponent);
        fmpz_clear(exponent);
        fmpz_clear(exp);
    }

    fmpz_clear(copy);
}

void strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(mpz_t f, std::string_view str) {
    strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(f, str, str.find('e'));
}

void strsciint2bigint_NULL_TERMINATED__NOT_THREADSAFE(mpz_t f, std::string_view str, size_t e_index) {
    assert(str.find('.') == std::string::npos);
    assert(str.at(e_index) == 'e');
    assert(str.at(e_index+1) != '-');

    mpz_init_set_strview(f, str.substr(0, e_index));

    std::string_view exp_str = str.substr(e_index+1);

    size_t exp10;
    if(ckd_10_exponent(exp10, exp_str) == false){
        mpz_mul_ui(f, f, exp10);
    }else if(ckd_str2int(&exp10, exp_str) == false && !spoof_bignum_path){
        mpz_t power_of_10;
        mpz_init(power_of_10);
        mpz_ui_pow_ui(power_of_10, 10, exp10);
        mpz_mul(f, f, power_of_10);
        mpz_clear(power_of_10);
    }else{
        DEBUG_DISABLE_SPOOF

        fmpz_t exp;
        fmpz_init_set_strview(exp, exp_str);

        fmpz_t ten;
        fmpz_init_set_ui(ten, 10);

        fmpz_t power_of_10;
        fmpz_init(power_of_10);
        fmpz_pow_fmpz(power_of_10, ten, exp);

        mpz_mul(f, f, COEFF_TO_PTR(*power_of_10));
        fmpz_clear(exp);
        fmpz_clear(power_of_10);
    }
}

void strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str) {
    strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(f, str, str.find('e'));
}

void strsciint2bigrat_NULL_TERMINATED__NOT_THREADSAFE(fmpq_t f, std::string_view str, size_t e_index) {
    assert(str.find('.') == std::string::npos);
    assert(str.at(e_index) == 'e');
    assert(str.at(e_index+1) == '-');

    fmpz_init_set_strview(fmpq_numref(f), str.substr(0, e_index));

    std::string_view exp_str = str.substr(e_index+2);

    size_t exp10;
    if(ckd_10_exponent(exp10, exp_str) == false){
        fmpz_init_set_ui(fmpq_denref(f), exp10);
    }else if(ckd_str2int(&exp10, exp_str) == false && !spoof_bignum_path){
        fmpz_init_set_ui(fmpq_denref(f), 10);
        fmpz_pow_ui(fmpq_denref(f), fmpq_denref(f), exp10);
    }else{
        DEBUG_DISABLE_SPOOF

        fmpz_t exp;
        fmpz_init_set_strview(exp, exp_str);

        fmpz_t ten;
        fmpz_init_set_ui(ten, 10);

        fmpz_init(fmpq_denref(f));
        fmpz_pow_fmpz(fmpq_denref(f), ten, exp);

        fmpz_clear(exp);
    }
}

}  // namespace KiCAS2

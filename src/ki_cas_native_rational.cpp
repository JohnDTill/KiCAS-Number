#include "ki_cas_native_rational.h"

#include "ki_cas_integer_math.h"
#include <cassert>
#include <numeric>

namespace KiCAS2 {

KiCAS2::NativeRational::NativeRational(size_t numerator, size_t denominator) noexcept
    : num(numerator), den(denominator) {
    assert(denominator != 0);
}

NativeRational::operator long double() const noexcept {
    return static_cast<long double>(num) / static_cast<long double>(den);
}

NativeRational::operator double() const noexcept {
    return static_cast<double>(num) / static_cast<double>(den);
}

NativeRational::operator float() const noexcept {
    return static_cast<float>(num) / static_cast<float>(den);
}

NativeRational::operator size_t() const noexcept {
    return num / den;
}

void NativeRational::reduceInPlace() noexcept {
    assert(den != 0);
    const size_t gcd = std::gcd(num, den);
    assert(gcd != 0);
    if(gcd != 1){
        num /= gcd;
        den /= gcd;
    }
}

bool ckd_mul(NativeRational* result, NativeRational a, size_t b) noexcept {
    result->den = a.den;
    if(ckd_mul(&result->num, a.num, b) == false) return false;

    // Resist expanding if possible
    const size_t gcd_a_den_b = std::gcd(a.den, b);
    if(gcd_a_den_b != 1){
        b /= gcd_a_den_b;
        result->den /= gcd_a_den_b;
        if(ckd_mul(&result->num, a.num, b) == false) return false;
    }

    const size_t gcd_a = std::gcd(a.num, a.den);
    if(gcd_a != 1){
        a.num /= gcd_a;
        result->den /= gcd_a;
        return ckd_mul(&result->num, a.num, b);
    }

    return true;
}

bool ckd_mul(NativeRational* result, NativeRational a, NativeRational b) noexcept {
    if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
        return false;

    const size_t gcd_a_num_b_den = std::gcd(a.num, b.den);
    if(gcd_a_num_b_den != 1){
        a.num /= gcd_a_num_b_den;
        b.den /= gcd_a_num_b_den;

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

    const size_t gcd_b_num_a_den = std::gcd(b.num, a.den);
    if(gcd_b_num_a_den != 1){
        b.num /= gcd_b_num_a_den;
        a.den /= gcd_b_num_a_den;

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

    const size_t gcd_a = std::gcd(a.num, a.den);
    if(gcd_a != 1){
        a.num /= gcd_a;
        a.den /= gcd_a;

        if(ckd_mul(&result->num, a.num, b.num) == false && ckd_mul(&result->den, a.den, b.den) == false)
            return false;
    }

    const size_t gcd_b = std::gcd(b.num, b.den);
    if(gcd_b != 1){
        b.num /= gcd_b;
        b.den /= gcd_b;

        return ckd_mul(&result->num, a.num, b.num) || ckd_mul(&result->den, a.den, b.den);
    }

    return true;
}

bool ckd_add(NativeRational* result, NativeRational a, size_t b) noexcept {
    // n/d + b = (n + b*d) / d

    size_t b_times_a_den;
    if(ckd_mul(&b_times_a_den, b, a.den)){
        const size_t gcd_a = std::gcd(a.num, a.den);

        if(gcd_a != 1){
            a.den /= gcd_a;
            if(ckd_mul(&b_times_a_den, b, a.den)) return true;
            a.num /= gcd_a;
        }
    }

    result->den = a.den;
    return ckd_add(&result->num, a.num, b_times_a_den);
}

bool ckd_add(NativeRational* result, NativeRational a, NativeRational b) noexcept {
    // a/b + c/d = (a*d + b*c) / (b*d)
    size_t a_num_times_b_den;
    size_t b_num_times_a_den;

    if(ckd_mul(&result->den, a.den, b.den) == false
       && ckd_mul(&a_num_times_b_den, a.num, b.den) == false
       && ckd_mul(&b_num_times_a_den, b.num, a.den) == false
       && ckd_add(&result->num, a_num_times_b_den, b_num_times_a_den) == false)
        return false;

    a.reduceInPlace();
    b.reduceInPlace();

    // (b*d) = [ a*(d/gcd(b,d)) + c*(b/gcd(b,d)) ] / [b*(d/gcd(b,d))]
    const size_t gcd_a_den_b_den = std::gcd(a.den, b.den);
    if(gcd_a_den_b_den != 1){
        a.den /= gcd_a_den_b_den;
        b.den /= gcd_a_den_b_den;
    }

    return ckd_mul(&result->den, a.den, b.den*gcd_a_den_b_den)
       || ckd_mul(&a_num_times_b_den, a.num, b.den)
       || ckd_mul(&b_num_times_a_den, b.num, a.den)
       || ckd_add(&result->num, a_num_times_b_den, b_num_times_a_den);
}

}  // namespace KiCAS2

#include "ki_cas_native_float.h"

#include <cassert>
#include <charconv>
#include <limits>

namespace KiCAS2 {

static size_t precision = 6;

void set_float_string_precision(size_t precision_val) noexcept {
    precision = precision_val;
}

size_t get_float_string_precision() noexcept {
    return precision;
}

void write_float(std::string& str, FloatingPoint val) {
#if !defined(__GNUC__) || __GNUC__ > 8
    constexpr size_t max_digits = std::numeric_limits<FloatingPoint>::max_digits10 + 8;
    char buffer[max_digits];
    const std::to_chars_result result = std::to_chars(buffer, buffer+max_digits, val, std::chars_format::general, precision);
    assert(result.ec == std::errc());
    str.append(buffer, result.ptr - buffer);
#else
    // Older GCC versions don't implement std::to_chars for floats
    str += std::to_string(val);
#endif
}

void write_float_term(std::string& str, FloatingPoint val){
    assert(str[str.size()-2] == '+');
    assert(str[str.size()-1] == ' ');
    if(val >= 0){
        write_float(str, val);
    }else{
        str[str.size()-2] = '-';
        write_float(str, -val);
    }
}

FloatingPoint strdecimal2floatingpoint(std::string_view str) noexcept {
    long double result;

#if !defined(__GNUC__) || __GNUC__ > 8
    const auto parse_result = std::from_chars(str.data(), str.data() + str.size(), result, std::chars_format::fixed);
    assert(parse_result.ptr == str.data()+str.size());
#else
    // Must allocate to avoid reading past end of view, e.g. parsing view of "23" from a string "1234"
    result = std::stold(std::string(str));
#endif

    return result;
}

FloatingPoint strscientific2floatingpoint(std::string_view str) noexcept {
    long double result;

#if !defined(__GNUC__) || __GNUC__ > 8
    const auto parse_result = std::from_chars(str.data(), str.data() + str.size(), result, std::chars_format::scientific);
    assert(parse_result.ptr == str.data()+str.size());
#else
    // Must allocate to avoid reading past end of view, e.g. parsing view of "23" from a string "1234"
    result = std::stold(std::string(str));
#endif

    return result;
}

}

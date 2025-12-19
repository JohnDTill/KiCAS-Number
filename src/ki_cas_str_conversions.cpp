#include "ki_cas_str_conversions.h"

#include <cassert>
#include <charconv>

namespace KiCAS2 {

void write_int(std::string& str, size_t val) {
    constexpr size_t max_digits = std::numeric_limits<size_t>::digits10 + 1;
    str.resize(str.size() + max_digits);

    // Append the number to the end of str
    char* end = str.data() + str.size();
    char* begin = end - max_digits;
    std::to_chars_result result = std::to_chars(begin, end, val);
    assert(result.ec == std::errc());

    // Trim size where prior allocation exceeded the actual need
    str.resize(result.ptr - str.data());
}

bool ckd_str2int(size_t* result, std::string_view str) noexcept {
    const auto parse_result = std::from_chars(str.data(), str.data() + str.size(), *result);
    assert(parse_result.ec != std::errc::invalid_argument);

    return parse_result.ec == std::errc::result_out_of_range;
}

void fmpz_set_str_NULL_TERMINATED_SOURCE__NOT_THREADSAFE(fmpz_t f, std::string_view str) {
    // Get the end of the std::string_view, violating the purported immutability of std::string_view!
    // The byte past "str" must be valid owned memory! (std::string is specified to be null-terminated since C++11)
    // This also means the string cannot be read from another thread during this operation.
    char* end_const_discarded = const_cast<char*>(str.data()+str.size());

    // Backup the character at the end, and replace with '\0' so that GMP knows to stop parsing the int.
    const char backup = *end_const_discarded;
    *end_const_discarded = '\0';

    // Parse the null-terminated str
    const auto code = fmpz_set_str(f, str.data(), 10);
    assert(code == 0);

    // Restore the original str
    *end_const_discarded = backup;
}



}

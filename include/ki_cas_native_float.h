#ifndef KI_CAS_NATIVE_FLOAT_H
#define KI_CAS_NATIVE_FLOAT_H

#include <string>

namespace KiCAS2 {

typedef long double FloatingPoint;

/// Set the precision when converting floats to strings
void set_float_string_precision(size_t precision_val) noexcept;

/// Get the precision when converting floats to strings
size_t get_float_string_precision() noexcept;

/// Append a float to the end of the string
void write_float(std::string& str, FloatingPoint val);

/// Append a float to the end of the string, handling the sign to write an addition term
void write_float_term(std::string& str, FloatingPoint val);

/// Parse a string to a floating point number
FloatingPoint strdecimal2floatingpoint(std::string_view str) noexcept;

/// Parse a string to a floating point number
FloatingPoint strscientific2floatingpoint(std::string_view str) noexcept;

}

#endif // KI_CAS_NATIVE_FLOAT_H

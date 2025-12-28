#ifndef KI_CAS_NUMBER_H
#define KI_CAS_NUMBER_H

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include <cstddef>
#include <gmp.h>
#include <flint/fmpq.h>
#include <flint/fmpz.h>
#include <string>

namespace KiCAS2 {

class Number {
public:
    void writeString(std::string& str) const;

private:
    struct Data {
    public:
        inline size_t getPosInt() const noexcept;
        inline void setPosInt(size_t val) noexcept;
        inline size_t getNegInt() const noexcept;
        inline void setNegInt(size_t val) noexcept;
        inline size_t getMachineInt() const noexcept;
        inline void setMachineInt(size_t val) noexcept;
        inline size_t getNumerator() const noexcept;
        inline void setNumerator(size_t val) noexcept;
        inline size_t getDenominator() const noexcept;
        inline void setDenominator(size_t val) noexcept;

    private:
        ~Data() noexcept;

        struct MachineRational {
            size_t numerator;
            size_t denominator;
        };

        struct Complex {
            Number* real;
            Number* imaginary;
        };

        union {
            size_t machine_int;
            MachineRational machine_rational;
            mpz_t big_int;
            fmpq_t big_rational;
            Complex complex;
            long double floating_point;
        } data;

        enum {
            PosInt,
            NegInt,
            PosRat,
            NegRat,
            BigInt,
            BigRat,
            Complex,
            Float,
        } tag;

        #ifndef NDEBUG
        bool isPosInt() const noexcept;
        bool isNegInt() const noexcept;
        bool isMachineInt() const noexcept;
        bool isMachineRat() const noexcept;
        #endif
    } data;
};

}  // namespace KiCAS2

#endif  // #ifndef KI_CAS_NUMBER_H

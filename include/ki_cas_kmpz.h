#ifndef KI_CAS_KMPZ_H
#define KI_CAS_KMPZ_H

#ifdef _MSC_VER
#include <malloc.h>  // MSC dependencies for GMP
#endif

#include <cinttypes>
#include "ki_cas_test_hooks.h"
#include <gmp.h>
#include <flint/fmpz.h>
#include <intx/intx.hpp>

namespace KiCAS2 {

typedef intx::uint128 uint128_t;
typedef intx::uint256 uint256_t;
typedef intx::uint512 uint512_t;

template<bool is_negative=false> void mpz_init_set_uint128(mpz_t lhs, uint128_t rhs);
template<bool is_negative=false> void mpz_init_set_uint256(mpz_t lhs, uint256_t rhs);
template<bool is_negative=false> void mpz_init_set_uint512(mpz_t lhs, uint512_t rhs);
template<bool is_negative=false> fmpz u128_to_fmpz(uint128_t val);
template<bool is_negative=false> fmpz u256_to_fmpz(uint256_t val);
void write_uint128(std::string& str, uint128_t val);
void write_uint256(std::string& str, uint256_t val);

}

#endif // KI_CAS_KMPZ_H

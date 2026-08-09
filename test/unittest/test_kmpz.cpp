#include <catch2/catch_test_macros.hpp>

#include "ki_cas_kmpz.h"

#include "ki_cas_big_num_wrapper.h"
#include <bit>
#include <iostream>

using namespace KiCAS2;

#ifdef IS_BIG_ENDIAN
TEST_CASE( "confirm_big_endian" ) {
    REQUIRE(std::endian::native == std::endian::big);
    std::cout << "Confirmed program under test is big-endian." << std::endl;
}
#else
TEST_CASE( "confirm_big_endian" ) {
    REQUIRE(std::endian::native == std::endian::little);
    std::cout << "Confirmed program under test is little-endian." << std::endl;
}
#endif

#ifdef IS_BIG_ENDIAN
#define CODE_BIG_ENDIAN(x) x
#define CODE_LITTLE_ENDIAN(x)  /* Do nothing */
#else
#define CODE_BIG_ENDIAN(x)  /* Do nothing */
#define CODE_LITTLE_ENDIAN(x) x
#endif

union TestUnion {
    CODE_LITTLE_ENDIAN( uint32_t u32; )
    CODE_LITTLE_ENDIAN( inline uint32_t& _32() noexcept { return u32; } )
    CODE_BIG_ENDIAN( struct U32 {uint32_t padding; uint32_t u32;} u32; )
    CODE_BIG_ENDIAN( inline uint32_t& _32() noexcept { return u32.u32; } )
    uint64_t u64;
    uint128_t u128;
    uint256_t u256;
    uint512_t u512;

    TestUnion() noexcept : u512(0){}
};

TEST_CASE( "union assumption - reading lesser size" ) {
    TestUnion val;
    val.u512 = 42;
    REQUIRE(val.u256 == 42);
    REQUIRE(val.u128 == 42);
    REQUIRE(val.u64 == 42);
    REQUIRE(val._32() == 42);
}

TEST_CASE( "union assumption - writing lesser size" ) {
    TestUnion val;
    val._32() = 42;
    REQUIRE(val.u512 == 42);
    REQUIRE(val.u256 == 42);
    REQUIRE(val.u128 == 42);
    REQUIRE(val.u64 == 42);

    TestUnion val2;
    val2.u64 = (uint64_t(1) << 50);
    REQUIRE(val2.u128 == (uint64_t(1) << 50));
    REQUIRE(val2.u256 == (uint64_t(1) << 50));
    REQUIRE(val2.u512 == (uint64_t(1) << 50));

    TestUnion val3;
    val3.u128 = (uint128_t(1) << 100);
    REQUIRE(val3.u256 == (uint128_t(1) << 100));
    REQUIRE(val3.u512 == (uint128_t(1) << 100));
}

TEST_CASE( "mpz_init_set_x" ){
    LEAK_CHECK_REQUIRE(isAllGmpMemoryFreed_resetIfNot());
    char buffer[256u] = { 0 };

    mpz_t val;
    mpz_init_set_ui(val, 42);
    REQUIRE(val->_mp_alloc == 1);
    REQUIRE(val->_mp_size == 1);
    REQUIRE(mpz_get_str(buffer, 10, val) == std::string("42"));
    mpz_clear(val);
    LEAK_CHECK_REQUIRE(isAllGmpMemoryFreed_resetIfNot());

    mpz_init_set_uint128(val, 42);
    REQUIRE(mpz_get_str(buffer, 10, val) ==
            std::string("42"));
    mpz_clear(val);
    LEAK_CHECK_REQUIRE(isAllGmpMemoryFreed_resetIfNot());

    mpz_init_set_uint128(val, uint128_t(1) << 100);
    REQUIRE(val->_mp_alloc == 16/sizeof(mp_limb_t));
    REQUIRE(val->_mp_size == 16/sizeof(mp_limb_t));
    REQUIRE(mpz_get_str(buffer, 10, val) ==
            std::string("1267650600228229401496703205376"));
    mpz_clear(val);
    LEAK_CHECK_REQUIRE(isAllGmpMemoryFreed_resetIfNot());
}

TEST_CASE( "to_fmpz" ){
    char buffer[256u] = { 0 };
    fmpz val;

    val = u128_to_fmpz(42);
    REQUIRE(fmpz_get_str(buffer, 10, &val) == std::string("42"));
    fmpz_clear(&val);

    val = u128_to_fmpz(uint128_t(1) << 100);
    REQUIRE(fmpz_get_str(buffer, 10, &val) ==
            std::string("1267650600228229401496703205376"));
    fmpz_clear(&val);

    val = u128_to_fmpz<true>(uint128_t(1) << 100);
    REQUIRE(fmpz_get_str(buffer, 10, &val) ==
            std::string("-1267650600228229401496703205376"));
    fmpz_clear(&val);

    val = u256_to_fmpz(42);
    REQUIRE(fmpz_get_str(buffer, 10, &val) == std::string("42"));
    fmpz_clear(&val);

    val = u256_to_fmpz(uint256_t(1) << 100);
    REQUIRE(fmpz_get_str(buffer, 10, &val) ==
            std::string("1267650600228229401496703205376"));
    fmpz_clear(&val);

    val = u256_to_fmpz<true>(uint256_t(1) << 100);
    REQUIRE(fmpz_get_str(buffer, 10, &val) ==
            std::string("-1267650600228229401496703205376"));
    fmpz_clear(&val);

    LEAK_CHECK_REQUIRE(isAllGmpMemoryFreed_resetIfNot());
}

TEST_CASE( "write_uint128" ){
    std::string str = "x + ";

    SECTION("Big value"){
        uint128_t val = (uint128_t(1) << 100);
        write_uint128(str, val);
        REQUIRE(str == "x + 1267650600228229401496703205376");
    }

    SECTION("Small value"){
        uint128_t val = 42;
        write_uint128(str, val);
        REQUIRE(str == "x + 42");
    }

    SECTION("Max value"){
        uint128_t val = std::numeric_limits<uint128_t>::max();
        write_uint128(str, val);
        REQUIRE(str == "x + 340282366920938463463374607431768211455");
    }

    SECTION("One digit less than max value"){
        uint128_t val = intx::from_string<uint128_t>(
            "99999999999999999999999999999999999999");
        write_uint128(str, val);
        REQUIRE(str == "x + 99999999999999999999999999999999999999");
    }

    SECTION("Zero padding (1)"){
        uint128_t val = intx::from_string<uint128_t>(
            "12345000000000000000054321");
        write_uint128(str, val);
        REQUIRE(str == "x + 12345000000000000000054321");
    }

    SECTION("Zero padding (2)"){
        uint128_t val = intx::from_string<uint128_t>(
            "300000000000012345000000000000000054321");
        write_uint128(str, val);
        REQUIRE(str == "x + 300000000000012345000000000000000054321");
    }
}

#include <catch2/catch_test_macros.hpp>

#include "ki_cas_native_float.h"

using namespace KiCAS2;

#if !defined(__GNUC__) || __GNUC__ > 8
TEST_CASE( "write_float" ){
    std::string out;

    SECTION("Trivial"){
        write_float(out, 0.0);
        REQUIRE(out == "0");
    }

    SECTION("Integer"){
        write_float(out, 42.0);
        REQUIRE(out == "42");
    }

    SECTION("Decimal"){
        write_float(out, 1.5);
        REQUIRE(out == "1.5");
    }

    SECTION("Scientific"){
        write_float(out, 2.998e8);
        REQUIRE(out == "2.998e+08");
    }
}
#endif

TEST_CASE( "strdecimal2floatingpoint" ){
    REQUIRE( std::abs(strdecimal2floatingpoint("4.2") - 4.2) < 1e-9 );
    REQUIRE( std::abs(strdecimal2floatingpoint("1234") - 1234.0) < 1e-9 );
}

TEST_CASE( "strdecimal2floatingpoint_valid_region" ){
    std::string str("1234");
    std::string_view view(str.data()+1, 2);

    REQUIRE( std::abs(strdecimal2floatingpoint(view) - 23.0) < 1e-9 );
}

TEST_CASE( "strscientific2floatingpoint" ){
    REQUIRE( std::abs(strscientific2floatingpoint("0.42e2")) - 42.0 < 1e-9 );
    REQUIRE( std::abs(strscientific2floatingpoint("12.34e-2")) - 0.1234 < 1e-9 );
}

TEST_CASE( "strscientific2floatingpoint_valid_region" ){
    std::string str("12e34");
    std::string_view view(str.data()+1, 3);

    REQUIRE( std::abs(strscientific2floatingpoint(view) - 2e3) < 1e-9 );
}

cmake_minimum_required(VERSION 3.5)

project(ki_cas_number VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
# pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)  # TODO: probably delete

if(UNIX)  # Linux/macOS
    # On Linux/macOS, the vcpkg Flint port provides a CMake config
    find_package(Flint REQUIRED)
elseif(MINGW)
    # On Windows, Flint has no CMake config or pkg-config. Need to handle manually.
    set(FLINT_ROOT "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-mingw-static")  # TODO: this must dynamically link
    include_directories("${FLINT_ROOT}/include")
    link_directories("${FLINT_ROOT}/lib")
endif()

set(SRC_FILES
    src/ki_cas_number.cpp
    src/ki_cas_number.h
)

add_library(ki_cas_number SHARED ${SRC_FILES})

# target_link_libraries(ki_cas_number PRIVATE PkgConfig::gmp)  # TODO: probably delete
# Flint and all its dependencies must be manually linked

if(UNIX)
    target_link_libraries(ki_cas_number PRIVATE Flint::flint)
elseif(MINGW)
    target_link_libraries(ki_cas_number PRIVATE flint gmp mpfr)
endif()

set_target_properties(ki_cas_number PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(ki_cas_number PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
set_target_properties(ki_cas_number PROPERTIES PUBLIC_HEADER src/ki_cas_number.h)

install(TARGETS ki_cas_number
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Testing setup
enable_testing()
add_executable(IntegrationTests test/test_driver.cpp ${SRC_FILES})
target_include_directories(IntegrationTests PUBLIC src)
get_target_property(LIBS ki_cas_number LINK_LIBRARIES)
target_link_libraries(IntegrationTests PRIVATE ${LINK_LIBRARIES})
add_test(NAME IntegrationTests COMMAND IntegrationTests)

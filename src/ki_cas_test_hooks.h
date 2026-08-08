#ifndef KI_CAS_TEST_HOOKS_H
#define KI_CAS_TEST_HOOKS_H

namespace KiCAS2 {

#ifndef PRIVATE
#define PRIVATE private
#endif

#ifndef NDEBUG
#define ASSERT(x) if(!(x)) throw("KiCAS assertion failed");
#define DEBUG_ONLY(x) x
#define NOEXCEPT_IN_RELEASE
#else
#define ASSERT(x)
#define DEBUG_ONLY(x)
#define NOEXCEPT_IN_RELEASE noexcept
#endif

/// Same as REQUIRE, but only count on failure
#define REQUIRE_NO_COUNT(x) \
    { const bool ok = x; if(!ok) REQUIRE(ok); }

}

#endif  // #ifndef KI_CAS_TEST_HOOKS_H

#ifndef ARCH_MACROS_H
#define ARCH_MACROS_H

namespace KiCAS2 {

#if defined(Word64)
    #if defined(Word32)
        #error "Defined both 64-bit and 32-bit"
    #endif
    static_assert(sizeof(size_t) == 8);
#elif defined(Word32)
    static_assert(sizeof(size_t) == 4);
#elif defined(__x86_64__) ||\
      defined(__aarch64__) ||\
      defined( _WIN64 ) ||\
      defined(__ppc64__) ||\
      defined(__s390x__)
    #define Word64
    static_assert(sizeof(size_t) == 8);
#else
    #define Word32
    static_assert(sizeof(size_t) == 4);
#endif

}  // namespace KiCAS2

#endif  // #ifndef ARCH_MACROS_H

/* Configuration for AndraOS */

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
  do {                                \
    builtin_define ("__andraos__");   \
    builtin_define ("__unix__");      \
    builtin_assert ("system=andraos");\
    builtin_assert ("system=unix");   \
  } while (0)

#undef LIB_SPEC
#define LIB_SPEC "-lc" /* link with -lc by default */

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0.o%s crti.o%s crtbegin.o%s"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

/* For Newlib */
#undef TARGET_LIBC_HAS_FUNCTION
#define TARGET_LIBC_HAS_FUNCTION default_libc_has_function

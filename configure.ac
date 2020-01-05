AC_INIT([bf], [1.0])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC

m4_include([m4/check_cflag.m4])

CHECK_CFLAG([-Wall])
CHECK_CFLAG([-Wextra])
CHECK_CFLAG([-Werror])
CHECK_CFLAG([-Wshadow])
CHECK_CFLAG([-Wrestrict])
CHECK_CFLAG([-Wformat=2])
CHECK_CFLAG([-Wlogical-op])
CHECK_CFLAG([-Wduplicated-cond])
CHECK_CFLAG([-Wdouble-promotion])
CHECK_CFLAG([-Wjump-misses-init])
CHECK_CFLAG([-Wnull-dereference])
CHECK_CFLAG([-Wduplicated-branches])
CHECK_CFLAG([-std=c89])
CHECK_CFLAG([-pedantic])

AC_SUBST([AM_CFLAGS])

AC_CONFIG_FILES([
  Makefile
])

AC_OUTPUT

AC_INIT([pghx], [0.1-alpha-4], [lisael@lisael.org])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AM_PROG_AR
AM_PATH_CHECK

LT_INIT

AC_PROG_CC
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile src/pghx/Makefile tests/Makefile])
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset pow select strdup strerror])

AC_CONFIG_SUBDIRS([pghx tests])
AC_SEARCH_LIBS([floor], [m])
AC_SEARCH_LIBS([timer_create], [rt])
AC_SEARCH_LIBS([pthread_create], [pthread])
AC_OUTPUT

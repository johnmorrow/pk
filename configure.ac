AC_PREREQ([2.68])
AC_INIT([pk],[1.0],[jmorrow@amoeba.org])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11 check-news foreign silent-rules subdir-objects])
AM_MAINTAINER_MODE
AM_PROG_CC_C_O
AC_CHECK_HEADERS([limits.h stdlib.h string.h])
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_HEADER(argp.h,,[AC_MSG_ERROR([Could not find argp.h])])
AC_CHECK_LIB(argp, argp_parse, ARGP_LIBS=-largp)
AC_SUBST([ARGP_LIBS])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

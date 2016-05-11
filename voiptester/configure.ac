#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([VoIP Server Tester], [0.1], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/cli/cli_main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create], ,AC_MSG_ERROR([Cannot find pthread library]))
AC_CHECK_LIB([termcap], [tputs], ,AC_MSG_ERROR([Cannot find termcap library]))
AC_CHECK_LIB([readline], [readline], ,AC_MSG_ERROR([Cannot find readline library]))

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero localtime_r memmove memset select socket strdup])

AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT

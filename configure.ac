#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([game_asset_packer], [0.0.1], [ade@arcadestuff.com])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
LT_INIT([disable-shared])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_REQUIRE_CPP
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_17([noext])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memset])

# X11
PKG_CHECK_MODULES(X11, [x11 xext], [HAVE_X11=1], [HAVE_X11=0])
AC_SUBST([X11_CFLAGS])
AC_SUBST([X11_LIBS])

AC_CONFIG_FILES([	Makefile 
									src/Makefile 
									src/adefs/Makefile 
									src/adepng/Makefile 
									src/window/Makefile 
								])

AC_OUTPUT


#   Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
#   This file is part of GTK+ IOStream class set
#
#   GTK+ IOStream is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   GTK+ IOStream is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You have received a copy of the GNU General Public License
#   along with GTK+ IOStream

# Author Matt Flax <flatmax@flatmax.org>

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([gtkiostream_current_version], [1]) # ++ if API added (age++), removed, or changed (age=0)
m4_define([gtkiostream_revision_version], [8]) # ++ if API same but src code change
m4_define([gtkiostream_age_version], [0]) # ++ if API added since last release, if API removed or changed then age=0
m4_define([gtkiostream_version],       [gtkiostream_current_version.gtkiostream_revision_version.gtkiostream_age_version])

AC_PREREQ([2.69])
AC_INIT( gtkIOStream, [gtkiostream_version], [ https://github.com/flatmax/gtkiostream/issues ])
AM_INIT_AUTOMAKE([dist-xz tar-ustar subdir-objects no-define])
AC_CONFIG_SRCDIR([include/Box.H])
AC_CONFIG_HEADERS([gtkiostream_config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT(/usr)
LT_INIT

AC_SUBST(LT_RELEASE, [gtkiostream_version])
AC_SUBST(LT_CURRENT, [gtkiostream_current_version:gtkiostream_revision_version:gtkiostream_age_version])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_F77

AC_DEFINE(GTKIOSTREAM_VERSION, [gtkiostream_version], [gtkIOStream package version number])

AC_OPENMP

# check for emscripten compilation
m4_ifdef([AM_EMSCRIPTEN], [AM_EMSCRIPTEN])
AM_CONDITIONAL(HAVE_EMSCRIPTEN, test x$emscripten_compiler = xyes)
if test "x$emscripten_compiler" = xyes; then
AC_SUBST([AM_CXXFLAGS], ["-s ERROR_ON_UNDEFINED_SYMBOLS=0 -s DEMANGLE_SUPPORT=1 -std=c++11 -O3 -fno-exceptions -s NO_EXIT_RUNTIME=1 -U EIGEN_DEBUG_ASSIGN"])
AC_DEFINE(HAVE_EMSCRIPTEN, [], [whether building for emsdk])
fi

AC_ARG_ENABLE(doc, AS_HELP_STRING([--disable-doc],
	      [Disable API documentation generation (default: auto)]),
	      [DOC=$enableval], [DOC=auto])

if test "x$DOC" = xauto ; then
# doxygen support
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
fi
# Checks for documentation
AM_CONDITIONAL([HAVE_DOXYGEN],[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([api/Doxyfile])])

#check whether this is a cygwin system
AM_CONDITIONAL(CYGWIN, test "$build_os" = "cygwin")

#check whether targeting mingw
case "$host" in
        *-*-mingw*|*-*-cygwin*)
                PLATFORM_STRING="Win32"
                ;;
        *-*-linux*)
                AC_DEFINE(PLATFORM_LINUX, 1, [Platform is Linux])
                PLATFORM_STRING="Linux"
                ;;
        *-*-darwin*)
		AC_MSG_WARN([Hello])
                PLATFORM_STRING="Apple"
                ;;
        *)
                AC_MSG_WARN([*** Please add $host to configure.ac checks!])
                ;;
esac
AM_CONDITIONAL(MINGW_SYSTEM, [test x$PLATFORM_STRING = xWin32])
AM_CONDITIONAL(NOT_MINGW_SYSTEM, [test x$PLATFORM_STRING != xWin32])

# Checks for ZeroC Ice or Icee
AC_LANG_PUSH(C++)
AC_MSG_CHECKING([for Icee])
AC_CACHE_VAL(ac_cv_zeroc_icee_check,  [AC_TRY_CPP([#include <IceE/IceE.h>],    ac_cv_zeroc_icee_check=yes,    ac_cv_zeroc_icee_check=no)])
  AC_MSG_RESULT($ac_cv_zeroc_icee_check)
  if test $ac_cv_zeroc_icee_check = yes; then
    AC_CHECK_LIB([rt], [main])
    AC_CHECK_LIB([IceE], [main])
    AC_PATH_PROG(SLICE2CPP, slice2cppe)
    AC_SUBST([SLICE2CPP])
    if test -z $SLICE2CPP; then
        AC_MSG_ERROR([ requires slice2cppe; (not in path)])
    fi
  else
    AC_MSG_CHECKING([for Ice])
    AC_CACHE_VAL(ac_cv_zeroc_ice_check,
      [AC_TRY_CPP([#include <Ice/Ice.h>], ac_cv_zeroc_ice_check=yes, ac_cv_zeroc_ice_check=no)])
      AC_MSG_RESULT($ac_cv_zeroc_ice_check)
      if test $ac_cv_zeroc_ice_check = yes; then
            AC_CHECK_LIB([Ice], [main])
            AC_CHECK_LIB([IceUtil], [main])
            AC_CHECK_LIB([rt], [main])
            AC_PATH_PROG(SLICE2CPP, slice2cpp)
            AC_SUBST([SLICE2CPP])
            if test -z $SLICE2CPP; then
                AC_MSG_ERROR([ requires slice2cpp; (not in path)])
            fi
      fi
fi
AM_CONDITIONAL(ZEROC_ICE_EMBEDDED, test x$ac_cv_zeroc_icee_check = xyes)
AM_CONDITIONAL(HAVE_ZEROC_ICE, test x$ac_cv_zeroc_icee_check = xyes)
if test "x$ac_cv_zeroc_icee_check" == xyes ; then
    AC_DEFINE(ZEROC_ICE_EMBEDDED, 1, [Define if ZeroC IceE is being used])
    AC_DEFINE(HAVE_ZEROC_ICE, 1, [Define if ZeroC IceE is being used])
fi
AM_CONDITIONAL(HAVE_ZEROC_ICE, test x$ac_cv_zeroc_ice_check = xyes)
if test "x$ac_cv_zeroc_ice_check" == xyes ; then
    AC_DEFINE(HAVE_ZEROC_ICE, 1, [Define if ZeroC Ice is being used])
fi


#glib
PKG_CHECK_MODULES([GLIB2], [glib-2.0], HAVE_GLIB2="yes", HAVE_GLIB2="no")
AC_SUBST(GLIB2_CFLAGS)
AC_SUBST(GLIB2_LIBS)
#HAVE_GLIB2="no"
AM_CONDITIONAL(HAVE_GLIB2, test x$HAVE_GLIB2 = xyes)
if test "x$HAVE_GLIB2" == xyes ; then
    AC_DEFINE(HAVE_GLIB2, [], [whether to build components relying on GLIB2])
fi

# gtkdatabox and gtk
PKG_CHECK_MODULES([GTK], [gtkdatabox], HAVE_GTK="yes", HAVE_GTK="no")
AM_CONDITIONAL(HAVE_GTKDATABOX, test x$HAVE_GTK = xyes)
AM_CONDITIONAL(HAVE_GTK, test x$HAVE_GTK = xyes)
PKG_CHECK_MODULES([GTK3], [gtk+-3.0], HAVE_GTK3="yes", HAVE_GTK3="no")
PKG_CHECK_MODULES([GTK2], [gtk+-2.0], HAVE_GTK2="yes", HAVE_GTK2="no")
AM_CONDITIONAL(HAVE_GTK2, test x$HAVE_GTK2 = xyes)
AM_CONDITIONAL(HAVE_GTK3, test x$HAVE_GTK3 = xyes)
if test "x$HAVE_GTK3" == xyes ; then
    AC_DEFINE(HAVE_GTK3, [], [whether to build components relying on GTK3])
else
if test "x$HAVE_GTK2" == xyes ; then
    AC_DEFINE(HAVE_GTK2, [], [whether to build components relying on GTK])
fi
fi
if test "x$HAVE_GTK" == xyes ; then
    AC_SUBST(GTK_CFLAGS)
    AC_SUBST(GTK_LIBS)
    AC_DEFINE(HAVE_GTK, [], [whether to build components relying on GTK])
    AC_DEFINE(HAVE_GTKDATABOX, [], [whether to build components relying on gtkdatabox])
else
# no gtkdatabox, check for gtk3 and then gtk2
PKG_CHECK_MODULES([GTK], [gtk+-3.0], HAVE_GTK="yes", HAVE_GTK="no")
#HAVE_GTK3="no"
AM_CONDITIONAL(HAVE_GTK, test x$HAVE_GTK = xyes)
if test "x$HAVE_GTK" == xyes ; then
    AC_SUBST(GTK_CFLAGS)
    AC_SUBST(GTK_LIBS)
    AC_DEFINE(HAVE_GTK, [], [whether to build components relying on GTK])
    AC_DEFINE(HAVE_GTK3, [], [whether to build components relying on GTK3])
else
PKG_CHECK_MODULES([GTK], [gtk+-2.0], HAVE_GTK="yes", HAVE_GTK="no")
#HAVE_GTK2="no"
AM_CONDITIONAL(HAVE_GTK, test x$HAVE_GTK = xyes)
if test "x$HAVE_GTK" == xyes ; then
    AC_SUBST(GTK_CFLAGS)
    AC_SUBST(GTK_LIBS)
    AC_DEFINE(HAVE_GTK, [], [whether to build components relying on GTK])
    AC_DEFINE(HAVE_GTK2, [], [whether to build components relying on GTK])
fi
fi
fi

# Checks for header files.
# eigen3
PKG_CHECK_MODULES(EIGEN, eigen3 >= 3.0.0,,AC_MSG_ERROR("Eigen3 is required for NeuralNetworks and Sox support - and possibly others"))
AC_SUBST(EIGEN_CFLAGS)
AC_SUBST(EIGEN_LIBS)
AC_DEFINE([EIGEN_FFTW_DEFAULT],[],[Using the FFTW backen with Eigen FFT])

# sox
PKG_CHECK_MODULES(SOX, sox,HAVE_SOX="yes", HAVE_SOX="no")
AC_SUBST(SOX_CFLAGS)
AC_SUBST(SOX_LIBS)
AM_CONDITIONAL(HAVE_SOX, test x$HAVE_SOX = xyes)
if test "x$HAVE_SOX" == xyes ; then
    AC_DEFINE(HAVE_SOX, [], [whether to build in Sox support])
fi

# alsa
PKG_CHECK_MODULES(ALSA, alsa, HAVE_ALSA="yes", HAVE_ALSA="no")
AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)
AM_CONDITIONAL(HAVE_ALSA, test x$HAVE_ALSA = xyes)
if test "x$HAVE_ALSA" == xyes ; then
    AC_DEFINE(HAVE_ALSA, [], [whether to build in Alsa support])
fi

# libwebsockets
PKG_CHECK_MODULES(LIBWEBSOCKETS, libwebsockets,HAVE_LIBWEBSOCKETS="yes", HAVE_LIBWEBSOCKETS="no")
AC_SUBST(LIBWEBSOCKETS_CFLAGS)
AC_SUBST(LIBWEBSOCKETS_LIBS)
AM_CONDITIONAL(HAVE_LIBWEBSOCKETS, test x$HAVE_LIBWEBSOCKETS = xyes)
if test "x$HAVE_LIBWEBSOCKETS" == xyes ; then
    AC_DEFINE(HAVE_LIBWEBSOCKETS, [], [whether to build in libwebsockets support])
fi

# check for cross compilation
AM_CONDITIONAL(WE_ARE_CROSS_COMPILING, [test "${cross_compiling}" = yes], [test "${cross_compiling}" = no])
if test "${cross_compiling}" = yes; then
    AC_DEFINE(WE_ARE_CROSS_COMPILING, [], [if cross compiling then don't build some tests])
fi
AC_ARG_ENABLE(octave,
	      AS_HELP_STRING([--enable-octave],
	      [Enable Octave functionality (default: disable)]),
	      [OCTAVE=$enableval], [OCTAVE=disable])
if test "x$OCTAVE" = xyes ; then

# Checks for octave
AC_CHECK_PROG([MKOCTFILE], [mkoctfile], [mkoctfile], [MKOCTFILE="no"])
if test "${cross_compiling}" = yes; then
    MKOCTFILE="no"
fi
# disable the mkoctfile program
AC_ARG_WITH([usemkoctfile],
  [  --with-usemkoctfile=no  Disable the use of the mkoctfile program.],
  [MKOCTFILE="$withval"])

if test "$MKOCTFILE" != "mkoctfile"; then
  #AC_PATH_PROG([mkoctfile],[mkoctfile],[no],[$prefix/bin:$PATH])
      # no mkoctfile program_invocation_name

    AC_ARG_WITH([octave-include-path],
    [--with-octave-include-path location of the octave headers, defaults to -I/usr/include/octave-3.8.1],
    [MKOCTFILE_CFLAGS="-I$withval"],
    [MKOCTFILE_CFLAGS='-I/usr/include/octave-3.8.1'])
    AC_ARG_WITH([octave-lib-path],
      [--with-octave-lib-path location of the octave libraries, defaults to -L/usr/lib/octave-3.8.1],
      [MKOCTFILE_LIBPATH="$withval"],
      [MKOCTFILE_LIBPATH='-L/usr/lib/octave-3.8.1'])
    AC_SUBST([MKOCTFILE_LIBPATH])

    AC_ARG_WITH([octave-lib-pc-path],
      [--with-octave-lib-pc-path location of the octave libraries for the .pc, defaults to -L/usr/lib/octave-3.8.1],
      [MKOCTFILE_PCLIBPATH="$withval"],
      [MKOCTFILE_PCLIBPATH='-L/usr/lib/octave-3.8.1'])
    AC_SUBST([MKOCTFILE_PCLIBPATH])

    AC_ARG_WITH([octave-libs],
      [--with-octave-libs location of the octave libraries, defaults to '-loctave -loctinterp'],
      [MKOCTFILE_LIBS="$withval"],
      [MKOCTFILE_LIBS='-loctave -loctinterp'])

    OCT_INC_DIR=($MKOCTFILE_CFLAGS)
    OCT_INC_DIR="$(echo ${OCT_INC_DIR[[0]]:2})"
    #AC_CHECK_FILE($OCT_INC_DIR/octave/octave.h, HAVE_OCTAVE="yes",HAVE_OCTAVE="no")
    HAVE_OCTAVE="yes"
else
    #AC_SUBST([MKOCTFILE],[$mkoctfile])
    AC_SUBST([MKOCTFILE])
    # get mkoctfile compiler flags
    CAT="cat"
    #if test "$cross_compiling" == "yes"; then
    #  CAT="dos2unix"
    #fi
    MKOCTFILE_CFLAGS="$(echo $($MKOCTFILE -p INCFLAGS | $CAT) | sed -re 's|\\|/|g')"
    MKOCTFILE_LIBS="$(echo \
      $(unset RLD_FLAG; $MKOCTFILE -p RLD_FLAG | $CAT) \
      $(unset LDFLAGS; $MKOCTFILE -p LDFLAGS | $CAT) \
      -L$(unset OCTLIBDIR; $MKOCTFILE -p OCTLIBDIR | $CAT) \
      $(unset LIBOCTINTERP; $MKOCTFILE -p LIBOCTINTERP | $CAT) \
      $(unset LIBOCTAVE; $MKOCTFILE -p LIBOCTAVE | $CAT) \
      $(unset LIBCRUFT; $MKOCTFILE -p LIBCRUFT | $CAT) \
      $(unset BLAS_LIBS; $MKOCTFILE -p BLAS_LIBS | $CAT) \
      $(unset FLIBS; $MKOCTFILE -p FLIBS | $CAT) \
      $(unset LIBS; $MKOCTFILE -p LIBS | $CAT) \
      | sed -re 's|\\|/|g')"

	OCT_INC_DIR=($MKOCTFILE_CFLAGS)
	OCT_INC_DIR="$(echo ${OCT_INC_DIR[[0]]:2})"

    AC_CHECK_FILE($OCT_INC_DIR/octave/octave.h, HAVE_OCTAVE="yes",HAVE_OCTAVE="no")
fi
else
    HAVE_OCTAVE="no"
fi

AM_CONDITIONAL(HAVE_OCTAVE, test x$HAVE_OCTAVE = xyes)
if test "x$HAVE_OCTAVE" == xyes ; then
    AC_SUBST([MKOCTFILE_CFLAGS])
    AC_SUBST([MKOCTFILE_LIBS])
    AC_DEFINE(HAVE_OCTAVE, [], [whether to build in Octave support])
else
    MKOCTFILE_CFLAGS=''
    MKOCTFILE_LIBS=''
    MKOCTFILE_LIBPATH=''
    MKOCTFILE_PCLIBPATH=''
fi

# jack
PKG_CHECK_MODULES([JACK], [jack], HAVE_JACK="yes", HAVE_JACK="no")
AC_SUBST(JACK_CFLAGS)
AC_SUBST(JACK_LIBS)
AM_CONDITIONAL(HAVE_JACK, test x$HAVE_JACK = xyes)
if test "x$HAVE_JACK" == xyes ; then
    AC_DEFINE(HAVE_JACK, [], [whether to build in Jack support])
fi

# fftw3
PKG_CHECK_MODULES([FFTW3], [fftw3, fftw3f],,AC_MSG_ERROR("fftw3 is required for building libgtkIOStream"))
AC_SUBST(FFTW3_CFLAGS)
AC_SUBST(FFTW3_LIBS)

# opencv
PKG_CHECK_MODULES(OPENCV,opencv, HAVE_OPENCV="yes", HAVE_OPENCV="no")
AM_CONDITIONAL(HAVE_OPENCV, test x$HAVE_OPENCV = xyes)
if test "x$HAVE_OPENCV" == xyes ; then
    AC_SUBST(OPENCV_CFLAGS)
    AC_SUBST(OPENCV_LIBS)
    AC_DEFINE(HAVE_OPENCV, [], [whether to build in openCV support])
else
    OPENCV_CFLAGS=''
    OPENCV_LIBS=''
fi

#HAVE_OCTAVE=yes
#AM_CONDITIONAL(HAVE_OCTAVE, test x$HAVE_OCTAVE = xyes)
#if test "x$HAVE_OCTAVE" == xyes ; then
#    AC_DEFINE(HAVE_OCTAVE, [], [whether to build in Octave support])
#fi

AM_CONDITIONAL(HAVE_OPENMP, false)
if test "x$OPENMP_CFLAGS" == x ; then
	AC_MSG_WARN([OpenMP not found])
else
	AC_MSG_WARN([OpenMP found])
	AM_CONDITIONAL(HAVE_OPENMP, true)
	AC_DEFINE(HAVE_OPENMP, [], [whether OpenMP is present])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile src/Makefile applications/Makefile api/Makefile test/Makefile include/Makefile src/deBoor/Makefile])

AC_OUTPUT(gtkIOStream.pc\
          gtkIOStreamORB.pc)

AC_MSG_NOTICE([])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([])
AM_COND_IF(HAVE_SOX, [AC_MSG_NOTICE([SOX ............................................ Present])], [AC_MSG_NOTICE([SOX ............................................ Not present
SOX tests and library support will not be built.])])

if test "x$HAVE_GTK3" == xyes ; then
AM_COND_IF(HAVE_GTK3, [AC_MSG_NOTICE([GTK3 ............................................ Present])], [AC_MSG_NOTICE([GTK3 ............................................ Not present
GTK3 tests and applications will not be built.])])
else
if test "x$HAVE_GTK2" == xyes ; then
AM_COND_IF(HAVE_GTK2, [AC_MSG_NOTICE([GTK2 ............................................ Present])], [AC_MSG_NOTICE([GTK2 ............................................ Not present
GTK tests and applications will not be built.])])
fi
fi

AM_COND_IF(HAVE_JACK, [AC_MSG_NOTICE([Jack ........................................... Present])], [AC_MSG_NOTICE([Jack ........................................... Not present
Jack tests and applications will not be built.])])

AM_COND_IF(HAVE_OCTAVE, [AC_MSG_NOTICE([Octave ....................................... Present])], [AC_MSG_NOTICE([Octave ......................................... Not present
Octave tests and applications will not be built.])])

AM_COND_IF(HAVE_ALSA, [AC_MSG_NOTICE([Alsa   ....................................... Present])], [AC_MSG_NOTICE([Alsa   ......................................... Not present
Alsa tests and applications will not be built.])])

AM_COND_IF(HAVE_LIBWEBSOCKETS, [AC_MSG_NOTICE([LIBWEBSOCKETS ............................................ Present])], [AC_MSG_NOTICE([LIBWEBSOCKETS ............................................ Not present
LIBWEBSOCKETS tests / library support will not be built.])])

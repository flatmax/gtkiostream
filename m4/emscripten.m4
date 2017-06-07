#   Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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
AC_DEFUN([AM_EMSCRIPTEN],
[
  AC_MSG_CHECKING(for emscripten compiler)
  emscripten_compiler=no
  AC_TRY_COMPILE([
      #ifndef EMSCRIPTEN
      #error "not an emscripten compiler"
      #endif
      ],[
      ],[
       host=javascript-web-emscripten
       emscripten_compiler=yes
       cross_compiling=yes
      ])
  AC_MSG_RESULT($emscripten_compiler)
  AM_CONDITIONAL(EMSCRIPTEN, [test "x$emscripten_compiler" = "xyes"])
])

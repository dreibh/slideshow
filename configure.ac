AC_INIT([Slideshow], [2.2.7~rc1], [dreib@iem.uni-due.de])

AM_INIT_AUTOMAKE
AC_PREREQ(2.60)


AC_PREFIX_DEFAULT(/usr/local)
if test "x$prefix" = "xNONE"; then
  prefix=$ac_default_prefix
  ac_configure_args="$ac_configure_args --prefix $prefix"
fi

AM_CONFIG_HEADER(config.h)


# ImageMagick

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# NOTE: Debian packaging uses GraphicsMagick,
#       with API compatible to ImageMagick!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

AC_MSG_CHECKING(Checking for ImageMagick header files)
AC_CHECK_HEADERS(magick/api.h)
MAGICK_CONFIG="Magick-config"
MAGICK_CFLAGS=`$MAGICK_CONFIG --cflags`
MAGICK_CXXFLAGS="$MAGICK_CFLAGS `$MAGICK_CONFIG --cppflags`"
MAGICK_LIBS=`$MAGICK_CONFIG --libs`
AC_SUBST(MAGICK_CFLAGS)
AC_SUBST(MAGICK_CXXFLAGS)
AC_SUBST(MAGICK_LIBS)


# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL


# colorgcc
AC_ARG_ENABLE([colorgcc],
              [  --enable-colorgcc       enable colorgcc support ][default=yes]],
              enable_colorgcc=$enableval, enable_colorgcc=yes)
if test "${enable_colorgcc}" = "yes"; then
   AC_CHECK_PROG([COLORGCC], [colorgcc], [yes], [no])
   if test "$COLORGCC" == "yes"
   then
      AC_MSG_NOTICE([Compiling using colorgcc])
      CC=colorgcc
      CXX=colorgcc -lstdc++
   fi
fi


# add here all your Makefiles. These will be created by configure
AC_OUTPUT(Makefile src/Makefile src/infrastructure/Makefile src/example/Makefile src/example/demo-images/Makefile )


echo ""
echo "The slideshow package has been configured with the following options:"
echo ""
echo "   CFLAGS                  : $CFLAGS"
echo "   CXXFLAGS                : $CXXFLAGS"
echo "   LDFLAGS                 : $LDFLAGS"
echo ""
echo "   MAGICK_CFLAGS           : $MAGICK_CFLAGS"
echo "   MAGICK_CXXFLAGS         : $MAGICK_CXXFLAGS"
echo "   MAGICK_LIBS             : $MAGICK_LIBS"

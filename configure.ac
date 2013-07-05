#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([Mutabor],[4.0.0beta3],[mutabor@mailbox.tu-dresden.de])

AC_ARG_ENABLE(XCode,
	AS_HELP_STRING([--enable-XCode],[Enable XCode compilation mode]),
	[XCode="$enableval"
])

AC_MSG_CHECKING(whether we are running from XCode)
if test -n "$XCode" -o -n "${XCODE_VERSION_MAJOR}"
then
	XCode=yes
	AC_MSG_RESULT($XCode)
	AC_MSG_CHECKING(XCode version)
	AC_MSG_RESULT(${XCODE_VERSION_MAJOR})
	ac_cv_prog_CC="gcc-$GCC_VERSION"
	ac_cv_prog_CXX="g++-$GCC_VERSION"
else 
	AC_MSG_RESULT(no)
fi

AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AC_USE_SYSTEM_EXTENSIONS
echo target_os: $target_os
AC_CONFIG_SRCDIR([README])
AC_CONFIG_MACRO_DIR([m4])


AM_INIT_AUTOMAKE([dist-bzip2 silent-rules subdir-objects -Wall])
AS_IF(test "${enable_silent_rules+set}" != set,
   enable_silent_rules=yes
   AM_DEFAULT_VERBOSITY=0)

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AM_SILENT_RULES
LT_INIT

AC_CONFIG_HEADER([config.h])


dnl PACKAGE=$PACKAGE_NAME
dnl VERSION=$PACKAGE_VERSION
PACKAGE_TARNAME=mutabor

AC_SUBST(PACKAGE)
AC_SUBST(VERSION)
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of this package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version of this package])

AC_DEFINE_UNQUOTED(PACKAGE_BUGREPORT, "$PACKAGE_BUGREPORT",
                                      [address for reporting bugs])

AM_OPTIONS_WXCONFIG


AH_TOP([
#ifndef MUTABOR_CONFIG_H_INCLUDED
#define MUTABOR_CONFIG_H_INCLUDED
/* We need this, because some autoconf tests rely on this (e.g. stpcpy)
    and it should be used for new programs anyway */
#define _GNU_SOURCE  1
])

AH_BOTTOM([
/* Debug also for Bison output. */
#ifdef DEBUG
/* #  define __WXDEBUG__ */
#  define YYDEBUG 1
#  ifndef _GLIBCXX_DEBUG
#    define _GLIBCXX_DEBUG
#  endif
#endif

#ifdef MUST_REUSE_C
#  define REUSE_C(x) x
#else
#  define REUSE_C(x)
#endif

#ifdef LOAD_LIBGNUINTL_H
#  include "libgnuintl.h"
#endif

#endif /* MUTABOR_CONFIG_H_INCLUDED */
])

wxflags=""
AM_MAINTAINER_MODE


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
PKG_PROG_PKG_CONFIG([0.14.0])

tmpcxxflags="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS -pipe"
AC_MSG_CHECKING([whether $CXX supports -pipe])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
		[AC_MSG_RESULT([yes])], 
		[CXXFLAGS="$tmpcxxflags"]
		[AC_MSG_RESULT([no])])
DEBUGCPPFLAGS=
dnl for final we could consider --repo to reduce object size
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug],[Enable extra debugging functionality]),
	[AS_IF(test "$enableval" = yes,
	       AC_DEFINE(DEBUG,1,[Define to 1 for compiling in debug mode.]),
               AC_DEFINE_UNQUOTED(DEBUG,[$enableval]))
	       CFLAGS="$CFLAGS -g"
	       CXXFLAGS="$CXXFLAGS -g -D_GLIBCXX_DEBUG -D__RTMIDI_DEBUG__"
	       AC_LANG_PUSH(C++)
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -Wall"
	       AC_MSG_CHECKING([whether $CXX supports -Wall])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -fdump-class-hierarchy"
	       AC_MSG_CHECKING([whether $CXX supports -fdump-class-hierarchy])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -fbounds-check"
	       AC_MSG_CHECKING([whether $CXX supports -fbounds-check])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -pedantic"
	       AC_MSG_CHECKING([whether $CXX supports -pedantic])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -fdiagnostics-show-option"
	       AC_MSG_CHECKING([whether $CXX supports -fdiagnostics-show-option])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -Wno-long-long"
	       AC_MSG_CHECKING([whether $CXX supports -Wno-long-long])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -Wno-variadic-macros"
	       AC_MSG_CHECKING([whether $CXX supports -Wno-variadic-macros])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -fsanitize=address"
	       AC_MSG_CHECKING([whether $CXX supports -fsanitize=address])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
	       tmpcxxflags="$CXXFLAGS"
	       CXXFLAGS="$CXXFLAGS -fno-omit-frame-pointer"
	       AC_MSG_CHECKING([whether $CXX supports -fno-omit-frame-pointer])
      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	       			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([no])])
dnl mudflap is not possible with g++ 4.7.1 and wxWidgets 2.8
dnl	       tmpcxxflags="$CXXFLAGS"
dnl	       tmplibs="$LIBS"
dnl	       CXXFLAGS="$CXXFLAGS -fmudflapth"
dnl	       LIBS="-lmudflapth"
dnl	       AC_MSG_CHECKING([whether $CXX supports  -fmudflapth -lmudflapth])
dnl      	       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
dnl	       			[AC_MSG_RESULT([yes])], 
dnl				[CXXFLAGS="$tmpcxxflags"
dnl				LIBS="$tmplibs"]
dnl	       			[AC_MSG_RESULT([no])])
dnl
	       case "$host_os" in 
	       darwin*)
	       	       tmpcxxflags="$CXXFLAGS"
	       	       CXXFLAGS="$CXXFLAGS -ggdb"
		       	       AC_MSG_CHECKING([whether $CXX supports -ggdb])
			       AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
					       	 [AC_MSG_RESULT([yes])], 
						 [CXXFLAGS="$tmpcxxflags"]
	       					 [AC_MSG_RESULT([no])])
			;;
	       esac
      	       AC_LANG_POP(C++)	       
	       wxflags="--debug"
])

AC_ARG_ENABLE(ansi,
	AS_HELP_STRING([--enable-ansi],[Enable strict ansi compiling and warnings]),
	[AS_IF(test "$enableval" = yes,
	       AC_DEFINE(ANSI,1,[Define to 1 for compiling in ansi mode.]),
               AC_DEFINE_UNQUOTED(ANSI,[$enableval]))
	 AX_CXXFLAGS_WARN_ALL_ANSI
	 AX_CFLAGS_WARN_ALL_ANSI
])
AC_ARG_ENABLE(gui,
	AS_HELP_STRING([--enable-gui],[Enable compiling ]),
	[AS_IF(test "$enableval" = yes,
	       AC_DEFINE(MUTWIN,1,[Define to 1 for compiling the gui version of $PACKAGE.]),
               AC_DEFINE_UNQUOTED(MUTWIN,[$enableval]))
],[
	       AC_DEFINE(MUTWIN,1,[Define to 1 for compiling the gui version of $PACKAGE.])
])
AM_WITH_DMALLOC
AC_DEFINE(RTMIDI,1,[Define to 1 for usage of the RtMidi Library.])
AC_PROG_LEX
AC_PROG_YACC
LT_INIT
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PATH_PROG(GIT,git)
AS_IF(test -n "$GIT",
	   tmpval=`$GIT --describe --tags 2>/dev/null`
	   AS_IF(test -z "$tmpval",GIT="")
)
AM_CONDITIONAL(COND_GIT, test -n "$GIT")
AC_PATH_PROG(PERL,perl)
AM_CONDITIONAL(COND_PERL, test -n "$PERL")
AS_IF(test -z "$PERL",
  AC_MSG_WARN([
          Some source files are generated using Perl. But I could not find
	  Perl. So changing those files will possibly have not the desired 
	  effect.

	  Simple compiling should work fine. Otherwise complain at 
	     $PACKAGE_BUGREPORT 
	  please.
]))
dnl this test must be implemented if necessary
AC_SUBST(SHUFFLE,["sort -R"])


CXX_FOR_SCOPING
AS_IF(test "$cxx_cv_iso_for_loop_scope" = yes,
  AC_DEFINE(REUSE(x), x, [Define if variables must be declared again in for loops]),
  AC_DEFINE(REUSE(x), , [Define if variables must be declared again in for loops]))
#./  AC_DEFINE_UNQUOTED(MUST_REUSE, [$cxx_iso_for_loop_scope]))
  
# Checks for libraries.
AC_CHECK_LIB(m,pow)

dnl check for boost earlier as it might correct the library path.
dnl some systems don't set it correctly
AX_BOOST_BASE
PKG_PROG_PKG_CONFIG

AC_CANONICAL_HOST
AC_MSG_CHECKING(for MIDI API)
AC_SUBST( midiapi, [""] )

copydlls=false

echo $host_os

case "$host_os" in 
dnl GNU/Linux on an Intel or compatible
*linux*)
	AC_LANG_PUSH(C++)
	tmpcxxflags="$CXXFLAGS"
	CXXFLAGS="$CXXFLAGS -fvisibility-inlines-hidden"
	AC_MSG_CHECKING([whether $CXX supports -fvisibility-inlines-hidden])
      	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[]])], 
	 			[AC_MSG_RESULT([yes])], 
				[CXXFLAGS="$tmpcxxflags"]
	       			[AC_MSG_RESULT([yes])])
    	AC_LANG_POP(C++)	       
	
	PLATFORM=linux
		
	AC_MSG_CHECKING(for MIDI API)
	AC_ARG_WITH(jack, [  --with-jack = choose JACK server support (mac and linux only)], [
 		midiapi="$midiapi -D__UNIX_JACK__"
		AC_MSG_RESULT(using JACK)
		PKG_CHECK_MODULES(JACK,[jack], , AC_MSG_ERROR(JACK support requires the jack library!) ) ], )

	# Look for ALSA flag
	AC_ARG_WITH(alsa, [  --with-alsa = choose native ALSA sequencer API support (linux only)], [
	midiapi="$midiapi -D__LINUX_ALSA__"
	AC_MSG_RESULT(using ALSA)
	AC_CHECK_LIB(asound, snd_seq_open, , AC_MSG_ERROR(ALSA support requires the asound library!))], )

	if [test "$midiapi" == "";] then
	    AC_MSG_RESULT(searching for ALSA and Jack)
	    AC_CHECK_LIB(asound, snd_seq_open)
	    if test "x$ac_cv_lib_asound_snd_seq_open" = xyes; then :
	      midiapi="$midiapi -D__LINUX_ALSA__"
	    fi
	    dnl the 4th argument to PKG_CHECK_MODULES should must be provided to prevent an error message, here
	    AS_IF(test -n "$PKG_CONFIG",
	   	    PKG_CHECK_MODULES(JACK,[jack],[midiapi="$midiapi -D__UNIX_JACK__"],))
	fi

	if [test "$midiapi" == "";] then
	    AC_MSG_ERROR(ALSA or Jack are needed to configure MIDI support!)
	fi

	# Checks for pthread library.
	AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(RtMidi requires the pthread library!))

	;;

darwin*)
	PLATFORM=macosx
	# Check for CoreAudio framework
	LIBS="$LIBS -framework CoreMIDI -framework CoreFoundation -framework CoreAudio"

	LDFLAGS="$LDFLAGS -framework Cocoa -framework Carbon -Wl,-Y,1455 -framework CoreAudio -framework CoreMIDI -framework CoreFoundation"

	  AC_ARG_WITH(jack, [  --with-jack = choose JACK server support (mac and linux only)], [
	    midiapi="$midiapi -D__UNIX_JACK__"
	    AC_MSG_RESULT(using JACK)
	    PKG_CHECK_MODULES(JACK,[jack], , AC_MSG_ERROR(JACK support requires the jack library!))], )

	 # Look for Core flag
	 AC_ARG_WITH(core, [  --with-core = choose CoreMidi API support (mac only)], [
	 	midiapi="$midiapi -D__MACOSX_CORE__"
		AC_MSG_RESULT(using CoreMidi)
		AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h, [], [AC_MSG_ERROR(CoreMIDI header files not found!)] )
    		LIBS="$LIBS -framework CoreMIDI -framework CoreFoundation -framework CoreAudio" ], )

	# If no api flags specified, use CoreMidi
	if [test "$midiapi" == "";] then
	   AC_MSG_RESULT(using both CoreMIDI and Jack if possible)
	   AC_CHECK_HEADER(CoreMIDI/CoreMIDI.h,[
		midiapi="$midiapi -D__MACOSX_CORE__"]
		LIBS="$LIBS -framework CoreMIDI -framework CoreFoundation -framework CoreAudio" 
	   , )
	   dnl the 4th argument to PKG_CHECK_MODULES should must be provided to prevent an error message, here
	   AS_IF(test -n "$PKG_CONFIG",
	   	    PKG_CHECK_MODULES(JACK,[jack],[midiapi="$midiapi -D__UNIX_JACK__"],))
	fi

	if [test "$midiapi" == "";] then
	    AC_MSG_ERROR(Core MIDI or Jack are needed to configure MIDI support!)
	fi

	;;

*winnt*|*interix*|cygwin*|*mingw*|uwin*)
	PLATFORM=windows
	copydlls=true
	LT_LANG([Windows Resource])
	AC_LIBTOOL_LANG_RC_CONFIG
	AC_CHECK_HEADERS([winsock2.h])
	AC_CHECK_LIB_HEADER(ws2_32,ntohs,[
	#include <winsock2.h>
	])
   	# Look for WinMM flag
	AC_ARG_WITH(winmm, [  --with-winmm = choose Windows MultiMedia (MM) API support (windoze only)], [
	    midiapi="$midiapi -D__WINDOWS_MM__"
	    ac_includes_default="$ac_includes_default 
#include <windows.h>
#include <mmsystem.h>
"
	    AC_LIB_WINMM([midiOutGetNumDevs])
	    AC_MSG_RESULT(using WinMM)
	    dnl LIBS="$LIBS -lwinmm 
	    ], )

	AC_ARG_WITH(winks, [  --with-winks = choose kernel streaming support (windoze only)], [
	    api="$api -D__WINDOWS_KS__"
	    AC_SUBST( LIBS, ["-lsetupapi -lksuser"] )
	    AC_MSG_RESULT(using kernel streaming) ], )

  # If no api flags specified, use WinMM
  if [test "$api" == "";] then
	    midiapi="$midiapi -D__WINDOWS_MM__"
	    ac_includes_default="$ac_includes_default 
#include <windows.h>
#include <mmsystem.h>
"
	    AC_LIB_WINMM([midiOutGetNumDevs])
	    AC_MSG_RESULT(using WinMM)
	    dnl LIBS="$LIBS -lwinmm 
  fi
	
	if test "$cross_compiling" = yes; then
		case "$target_os" in 
		mingw*|cygwin*)
			AC_MSG_NOTICE([
  Cross compiling for windows. Assuming you use a compiler
  with gcc style malloc and realloc.
            ])
			ac_cv_func_malloc_0_nonnull=yes
			ac_cv_func_realloc_0_nonnull=yes
			;;
		*)
			AC_MSG_WARN([
			Cross compiling for $target_os can not detect your 
			compiler behavior. Please, contact the author and help
			supplying needed information!
			])
		esac
	fi
	;;
*)
	AC_MSG_ERROR([
		Your target system could not be recognized. $PACKAGE depends on
		a working MIDI configuration for your System. So it can't be built
		without knowing it.
		
		Please contact the Development team at 
		    
		    $PACKAGE_BUGREPORT

		and provide the following information:

		* The output of configure
		* The description of your System.

	])

esac
CPPFLAGS="$CPPFLAGS $midiapi"
AM_CONDITIONAL(COPYDLLS,$copydlls)

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18])
AM_GNU_GETTEXT_INTL_SUBDIR
dnl wxMenuItem::GetItemLabel is implemented from 2.8.5
AM_PATH_WXCONFIG(2.8.5, wxWin=1, wxWin=0, [aui,adv,core,html,xrc,stc,xml],[$wxflags])
if test "$wxWin" != 1 -a -z "$XCode"; then
         AC_MSG_ERROR([
                wxWindows must be installed on your system
                but wx-config script couldn't be found.
     
                Please check that wx-config is in path, the directory
                where wxWindows libraries are installed (returned by
                'wx-config --libs' command) is in LD_LIBRARY_PATH or
                equivalent variable and wxWindows version is 2.8.0 or above.
        ])
	AC_SUBST(USE_INCLUDED_LIBWXINTL,no)
	AC_SUBST(BUILD_INCLUDED_LIBWXINTL,no)
else
	AC_DEFINE(WX,1,[Define to 1 if $PACKAGE should be compiled using wxWidgets.])
	AC_SUBST(INTLDIR,wxintl)
	AC_SUBST(USE_INCLUDED_LIBWXINTL,yes)
	AC_SUBST(BUILD_INCLUDED_LIBWXINTL,yes)
	AC_SUBST(USE_INCLUDED_LIBINTL,no)
	AC_SUBST(BUILD_INCLUDED_LIBINTL,no)
	enable_nls="no"
fi

case "`basename "$WX_CONFIG_PATH"`" in
inplace-*)
	wx_is_inplace=yes
	;;
*)
	wx_is_inplace=no
esac
AM_CONDITIONAL(WX_INPLACE,test "$wx_is_inplace")

AS_IF(test "$wx_is_inplace" = yes,
	   AC_CACHE_CHECK([for wxWidgets locale directories], [ac_cv_wx_locale_dirs],
	   [
	   	ac_cv_wx_locale_dirs="`$WX_CONFIG_WITH_ARGS --prefix`/locale"
	   ]),
	   AC_CACHE_CHECK([for wxWidgets locale directories], [ac_cv_wx_locale_dirs],
	   [
	
		for dir in `$WX_CONFIG_WITH_ARGS --prefix` \
		    /usr /usr/local /opt /opt/local ; 
		do 
			if test -d "$dir"/share/locale  ; then
			   found=""
			   for dirx in "$dir"/share/locale/* 
			   do 
			      test -f "$dirx"/LC_MESSAGES/wxstd.mo && found="yes"
			   done

			   if test "$found" = yes ; then
			      ac_cv_wx_locale_dirs="$ac_cv_wx_locale_dirs $dir/share/locale"
			   fi
			fi	
		done
	  ])
)
AC_SUBST(WX_LOCALE_DIRS,$ac_cv_wx_locale_dirs)


AC_CACHE_CHECK([where to find support dlls], [ac_cv_mingw_dll_dirs],
[
       SEARCHDIRS=`$WX_CONFIG_PATH --exec-prefix`/lib
       tmpsearch=`$CXX $CXXFLAGS -print-search-dirs|\
       			sed -e '/^libraries:/ { s/^libraries: *=\?//;s/:/ /g; p } ; d'`
       SEARCHDIRS="$SEARCHDIRS $tmpsearch"
       for d in /usr/share/doc/mingw32-runtime /mingw/lib /mingw/bin  /usr/lib /lib /usr/bin /bin
       do 
       	  AS_IF(test -d $d,
	  [ SEARCHDIRS="$SEARCHDIRS $d" ])
       done
       ac_cv_mingw_dll_dirs="$SEARCHDIRS"
])

AC_SUBST(DLLSEARCHPATH,"$ac_cv_mingw_dll_dirs")

AS_IF(test -n "$WX_RESCOMP",RC="$WX_RESCOMP")
AM_CONDITIONAL(COND_WINRC,test -n "$RC")
AS_IF(test "$PLATFORM" = "macosx",[
	COND_WINRC_TRUE='#'
	COND_WINRC_FALSE=
])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_CHECK_HEADERS_ONCE([limits.h stddef.h math.h netinet/in.h sys/time.h malloc.h mcheck.h])
ac_includes_default="$ac_includes_default 
#if HAVE_MALLOC_H
#  include <malloc.h>
#endif
"

echo \|$HAVE_NETINET_IN_H\|
if test "$ac_cv_header_netinet_in_h" = no ; then
	AC_SUBST(NETINET_INC,"-I\$(top_srcdir)/includes/netinet")
fi
if test "$HAVE_MCHECK_H"; then
	AC_SUBST(MCHECK,"mcheck")
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_RESTRICT
AC_C_VOLATILE
AC_C_INLINE

AC_LANG_PUSH(C)
if test "$ac_cv_c_inline" != no ; then
  AC_DEFINE(HAVE_INLINE,1,[Define to 1 if the C Compiler supports inline functions.])
  AC_SUBST(HAVE_INLINE)
fi

dnl Check for "extern inline", using a modified version
dnl of the test for AC_C_INLINE from acspecific.mt
dnl
AC_CACHE_CHECK([for extern inline], ac_cv_c_extern_inline,
[ac_cv_c_extern_inline=no
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[extern $ac_cv_c_inline double foo(double x);
extern $ac_cv_c_inline double foo(double x) { return x+1.0; };
double foo (double x) { return x + 1.0; };]], [[  foo(1.0)  ]])],[ac_cv_c_extern_inline="yes"],[])
])

if test "$ac_cv_c_extern_inline" != no ; then
  AC_DEFINE(HAVE_EXTERN_INLINE,1,[Define to 1 if "extern inline" works.])
  AC_SUBST(HAVE_EXTERN_INLINE)
fi

AC_CACHE_CHECK([for variable scope in for loops in C mode], ac_cv_c_scope_reuse,
[
 AC_COMPILE_IFELSE([
	AC_LANG_PROGRAM([],[
	for (int i=1; i<3; i++) ;
	for (int i=1; i<4; i++) ; 
	exit (0);
	])],[ac_cv_c_scope_reuse=inside],[ac_cv_c_scope_reuse=outside])
])

if test $ac_cv_c_scope_reuse = inside
then
	AC_DEFINE(MUST_REUSE_C,1,[Define to x if the scope of C loop variables is inside the loop])
fi




AC_LANG(C++)

dnl AC_CACHE_CHECK([for variable scope in for loops in C++ mode], ac_cv_cxx_scope_reuse,
dnl [
dnl AC_COMPILE_IFELSE([
dnl	AC_LANG_PROGRAM([],[
dnl	for (int i=1; i<3; i++) ;
dnl	for (int i=1; i<4; i++) ; 
dnl	exit(0);
dnl	])],[ac_cv_cxx_scope_reuse=inside],[ac_cv_cxx_scope_reuse=outside])
dnl ])
dnl if test $ac_cv_cxx_scope_reuse = inside
dnl then
dnl	AC_DEFINE(MUST_REUSE,1,[Define to x if the scope of C++ loop variables is inside the loop])
dnl fi

AX_BOOST_BASE
tmpcppflags="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
AC_CHECK_HEADERS([boost/intrusive_ptr.hpp])
AC_CHECK_HEADERS([boost/rational.hpp])
AC_CHECK_HEADERS([boost/spirit/core.hpp])
AC_CHECK_HEADERS([boost/spirit/actor/push_back_actor.hpp])
CPPFLAGS="$tmpcppflags"
AC_LANG_POP(C++)

AC_CACHE_SAVE
AC_ARG_ENABLE(precompile,
	AS_HELP_STRING([--enable-precompile],[use precompiled headers]),
	[ AX_COMPILER_PCH_CXX
	  AX_COMPILER_PCH_C
	  ],[
		AM_CONDITIONAL(COND_PRECOMP_C,false)
		AM_CONDITIONAL(COND_PRECOMP_CXX,false)
	  ])

# Checks for library functions.
AM_INTL_SUBDIR
AC_FUNC_MALLOC
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove pow getcwd gettimeofday memset select stpcpy strchr strstr]) 
AC_CHECK_FUNC(strdup)
AC_FUNCTION_VECTOR_DATA

# Locate CppUnit (minimum version 1.8.0) for testing.  
AM_PATH_CPPUNIT(1.8.0)

# You can set up an automake conditional and use it to conditionally
# build cppunit-using test programs.
AM_CONDITIONAL(HAVE_CPPUNIT, test "$CPPUNIT_LIBS")
CPPUNIT_CFLAGS="$CPPUNIT_CFLAGS -DMUTABOR_CPPUNIT -DMUTABOR_TEST"

dnl non verbose

dnl AC_SUBST(top_srcdir_nsi,[`echo "$srcdir"|sed -e 's,/,\\\\,g'`])
AC_SUBST(top_srcdir_nsi,[`echo "$srcdir"|sed -e 's,/,/,g'`])

function abspath {
	if [[ -d "$1" ]]
	then
		pushd "$1" >/dev/null
		pwd
		popd >/dev/null
	elif [[ -e $1 ]]
	then
		pushd $(dirname $1) >/dev/null
		echo $(pwd)/$(basename $1)
		popd >/dev/null
	else
		echo $1 does not exist! >&2
		return 127
	fi
}

dnl preserve some variables for cross compiling
# propagate selected configure variables to DISTCHECK_CONFIGURE_FLAGS 
for v in CPPFLAGS CXX CXXFLAGS CXXCPP LDFLAGS LIBS; do 
    eval s=\${$v+set}
    if test "$s" = set
    then 
       eval val=\$`echo $v`
       preserve_flags="${preserve_flags}'${v}=${val}' " 
    fi 
done 
# propagate Host triplets to DISTCHECK_CONFIGURE_FLAGS 
for v in host build target; do 
    tmp="${v}_alias"
    eval s=\${$tmp+set}
    if test "$s" = 'set' ; then 
       eval val=\$`echo $tmp`
       preserve_flags="${preserve_flags}'--${v}=${val}' " 
    fi 
done 
# propagate certain configure options to DISTCHECK_CONFIGURE_FLAGS 
for v in with-wx-config with-wx-dir with-boost with-cppunit-prefix with-cppunit-exec-prefix ; do 
    tmp="`echo "$v" | sed -e 's/-/_/g'`"
    eval s=\${$tmp+set}
    if test "$s" = 'set' ; then 
       eval val=\$`echo $tmp`
       val=`abspath "$val"`
       preserve_flags="${preserve_flags}'--${v}=${val}' " 
    fi 
done 
DISTCHECK_CONFIGURE_FLAGS="$preserve_flags"

AC_SUBST(DISTCHECK_CONFIGURE_FLAGS)  
AC_SUBST(LIBS)

AC_CONFIG_FILES([  
po/Makefile.in 
Makefile
src/Makefile
src/gmut/Makefile
src/gmut/tests/Makefile
src/includes/Makefile 
src/includes/mutabor/Makefile
src/includes/mutabor/tests/Makefile
src/includes/netinet/Makefile
src/includes/tests/Makefile
src/kernel/Makefile
src/kernel/routing/gmn/Makefile
src/kernel/routing/gmn/tests/Makefile
src/kernel/routing/Makefile
src/kernel/routing/midi/Makefile
src/kernel/routing/midi/tests/Makefile
src/kernel/routing/tests/Makefile
src/kernel/tests/Makefile
src/tests/Makefile
src/wxGUI/generic/Makefile
src/wxGUI/generic/tests/Makefile
src/wxGUI/Makefile
src/wxGUI/Routing/Makefile
src/wxGUI/Routing/tests/Makefile
src/wxGUI/tests/Makefile
src/wxintl/Makefile
src/wxintl/tests/Makefile
src/xrc/Makefile
src/xrc/tests/Makefile
lib/Makefile
lib/missfunc/Makefile
intl/Makefile
osdep/macosx/Info.plist
osdep/win/Mutabor.nsi
doc/Makefile
doc/mutabor.1
doc/localized/Makefile
doc/localized/de/Makefile
doc/localized/en/Makefile
Doxyfile
osdep/linux/OpenSuse.spec
tools/midi2text/Makefile
])
dnl  src/libmutabor/lextables/Makefile
dnl  src/libmutabor/lextables/tests/Makefile
dnl  src/libmutabor/Makefile
dnl  src/libmutabor/tests/Makefile
dnl  dnl AC_CONFIG_FILES([
dnl libmutabor/Makefile 
dnl libmutabor/tests/Makefile 
dnl includes/mutabor/Makefile 
dnl ])
AC_OUTPUT


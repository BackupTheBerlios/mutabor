include $(top_srcdir)/common-rules.mk
include $(top_srcdir)/common-linklibs.mk

check_PROGRAMS=timer1

TESTS=timer1$(EXEEXT)

timer1_SOURCES = \
	timer1.cpp

nodist_timer1_SOURCES = \
	debugPaths.cpp

timer1_LDADD= \
	$(top_builddir)/libmutabordebugtest.a \
	$(top_builddir)/libmutaborgui.a \
	$(top_builddir)/libmutaborrouting.a \
	$(top_builddir)/libmutaborkernel.a \
	$(top_builddir)/librtmidi.a \
	$(top_builddir)/src/wxintl/libwxintl.a \
	$(LDADD) \
	$(AM_LDFLAGS)

if HAVE_CPPUNIT
TESTS += \
	testcppunit$(EXEEXT)

check_PROGRAMS += testcppunit

testcppunit_CXXFLAGS = $(AM_CXXFLAGS) $(CPPUNIT_CFLAGS)

testcppunit_SOURCES = \
	main.cpp \
	timing.h \
	RouteTest.h \
	RouteTest.cpp \
	BoxTest.h \
	BoxTest.cpp \
	CommonFileDeviceTest.h

nodist_testcppunit_SOURCES = \
	debugPaths.cpp

testcppunit_LDADD = \
	$(top_builddir)/libmutabormaincppunit.a \
	$(top_builddir)/libmutabordebugcppunit.a \
	$(top_builddir)/libmutaborgui.a \
	$(top_builddir)/libmutaborrouting.a \
	$(top_builddir)/libmutaborkernel.a \
	$(top_builddir)/librtmidi.a \
	$(top_builddir)/src/wxintl/libwxintl.a \
	$(LDADD) \
	$(AM_LDFLAGS) \
	$(CPPUNIT_LIBS)
endif



if COPYDLLS
check-am: check-dll

check-dll: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS)	linkchecks

linkchecks:
	for d in `echo $(TESTS)| tr ' ' '\n' | grep -i -e '$(EXEEXT)$$'` ; \
	do \
		ls -l "$$d" ; \
		$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE="$$d" DLLEXEDIR="." installdll ; \
		test -f ".libs/$$d" && \
			$(MAKE) $(AM_MAKEFLAGS) DLLLINKFILE=".libs/$$d" DLLEXEDIR="." installdll ; \
	done
endif


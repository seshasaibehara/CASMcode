#===========================================================#
#To understand why it's structured this way see
#http://karelzak.blogspot.co.uk/2013/02/non-recursive-automake.html

ACLOCAL_AMFLAGS = -I build-aux/m4

#===========================================================#
#Start out empty, let the Makemodule.am files fill these in
lib_LTLIBRARIES=
check_LTLIBRARIES=
noinst_LIBRARIES=
bin_PROGRAMS=
check_PROGRAMS=
man1_MANS=
dist_bin_SCRIPTS=
TESTS=

#===========================================================#
#Define flags for everything ever
TXT_VERSION:=$(shell cat $(top_srcdir)/casm_version.txt)

AM_CXXFLAGS = -DTXT_VERSION='"$(TXT_VERSION)"'\
			  -DEIGEN_DEFAULT_DENSE_INDEX_TYPE=long\
			  -DGZSTREAM_NAMESPACE=gz

AM_CPPFLAGS = -I$(srcdir)/include/casm/external/qhull/libqhull_r/\
			  -I$(srcdir)/include/casm/external/gzstream\
			  -I$(srcdir)/include/casm/external/eigen\
			  -I$(srcdir)/submodules/googletest/googletest\
			  -I$(srcdir)/submodules/googletest/googletest/include\
			  -I$(srcdir)/include\
			  -I$(srcdir)/include/casm/external/gzstream\
			  -I$(srcdir)/include/casm/external/qhull/libqhullcpp
			  $(BOOST_CPPFLAGS)

AM_LDFLAGS = $(BOOST_LDFLAGS)

BUILT_SOURCES=

#============================================================#
#Files that arent sources for executalbes or headers that get installed, but are needed in the distribution.
#Even though the include/casm/external is distributed through headers, the licensing and other files should
#also be included.
EXTRA_DIST=

EXTRA_DIST+=./casm_version.txt\
			./man\
			./sample_input_files\
      LICENSE

#Extra files that should be cleaned up upon `distclean`
DISTCLEANFILES=

#============================================================#
#Always have included modules use paths relative to this
#parent Makefile.am. Avoid variable expansion in *_SOURCES
include $(srcdir)/apps/ccasm/Makemodule.am
include $(srcdir)/apps/completer/Makemodule.am
include $(srcdir)/include/casm/Makemodule.am
include $(srcdir)/include/ccasm/Makemodule.am
include $(srcdir)/src/casm/Makemodule.am
include $(srcdir)/src/ccasm/Makemodule.am
include $(srcdir)/tests/unit/Makemodule.am
include $(srcdir)/tests/functional/Makemodule.am
#============================================================#

#Misc targets
.FORCE:

test:
	echo "srcdir: $(srcdir)"
	echo "top_srcdir: $(top_srcdir)"
	echo "AM_LDFLAGS: $(AM_LDFLAGS)"
	echo "AM_CXXFLAGS: $(AM_CXXFLAGS)"
	echo "AM_CPPFLAGS: $(AM_CPPFLAGS)"
	echo "casm_LDFLAGS: $(casm_LDFLAGS)"
	echo "BOOST_LDFLAGS: $(BOOST_LDFLAGS)"
	echo "BOOST_CXXFLAGS: $(BOOST_CXXFLAGS)"
	echo "BOOST_CPPFLAGS: $(BOOST_CPPFLAGS)"
	echo "includedir: $(includedir)"
	echo "TXT_VERSION: $(TXT_VERSION)"
	echo "bin_PROGRAMS: $(bin_PROGRAMS)"
	echo "top_builddir: $(top_builddir)"
	echo "abs_top_builddir: $(abs_top_builddir)"
	echo "abs_srcdir: $(abs_srcdir)"

#============================================================#
#Recurse into these places (must be direct children, don't be fancy)
#SUBDIRS= ##DO NOT USE THIS. SERIOUSLY.##

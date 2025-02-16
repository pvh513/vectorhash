.PHONY: all default lib32 testclean clean distclean check check32 install

CXX = g++
CXXFLAGS = -g -W -Wall -Wno-unused-command-line-argument -ansi -std=c++11 -O3 -funroll-loops
LDFLAGS = -lvhsum -Llib64

INSTALLDIR = /usr/local
LIBDIR64 = lib64
LIBDIR32 = lib

RANLIB := ${shell which ranlib}
ifneq ($(RANLIB),)
  RANLIB = ranlib
else
  RANLIB = ar ts
endif

GZIP := ${shell which gzip}
ifneq ($(GZIP),)
  GZIP = gzip
  GZEXT = .gz
else
  GZIP = touch
  GZEXT =
endif

# DEP_GOALS will be an empty string for targets that don't need Makefile.dep
NODEP_GOALS = testclean clean distclean install
DEP_GOALS = $(filter-out $(NODEP_GOALS),$(MAKECMDGOALS))

# the default target does need Makefile.dep
ifeq ($(MAKECMDGOALS),)
  DEP_GOALS = need_dependencies
endif

ifneq ($(DEP_GOALS),)
  $(shell script/generate_deps.sh "$(CXX)" "$(CXXFLAGS)" > Makefile.dep)
endif

all: default

default: bin/vh32sum bin/vh64sum bin/vh128sum bin/vh256sum bin/vh512sum bin/vh1024sum lib64/libvhsum.a

lib32: lib32/libvhsum.a

testclean:
	cd tests; \
	$(MAKE) clean

clean: testclean
	rm -f Makefile.dep
	rm -f lib64/*.o
	rm -f lib64/*/*.o
	rm -f lib32/*.o
	rm -f lib32/*/*.o
	rm -f lib64/libvhsum.a
	rm -f lib32/libvhsum.a
	rm -f bin/vh32sum
	rm -f bin/vh64sum
	rm -f bin/vh128sum
	rm -f bin/vh256sum
	rm -f bin/vh512sum
	rm -f bin/vh1024sum

distclean: clean
	cd unittest-cpp; \
	$(MAKE) clean

bin/vh32sum: lib64/vectorhash.o lib64/libvhsum.a
	$(CXX) lib64/vectorhash.o $(LDFLAGS) -o $@

bin/vh64sum: bin/vh32sum
	ln -f bin/vh32sum bin/vh64sum

bin/vh128sum: bin/vh32sum
	ln -f bin/vh32sum bin/vh128sum

bin/vh256sum: bin/vh32sum
	ln -f bin/vh32sum bin/vh256sum

bin/vh512sum: bin/vh32sum
	ln -f bin/vh32sum bin/vh512sum

bin/vh1024sum: bin/vh32sum
	ln -f bin/vh32sum bin/vh1024sum

unittest-cpp/lib64/libUnitTest++.a:
	cd unittest-cpp; \
	$(MAKE)

unittest-cpp/lib32/libUnitTest++.a:
	cd unittest-cpp; \
	$(MAKE) lib32

check: unittest-cpp/lib64/libUnitTest++.a lib64/libvhsum.a
	cd tests; \
	$(MAKE)

check32: unittest-cpp/lib32/libUnitTest++.a lib32/libvhsum.a
	cd tests; \
	$(MAKE) check32

install:
	mkdir -p $(INSTALLDIR)/bin
	cp -af bin/vh*sum $(INSTALLDIR)/bin
	mkdir -p $(INSTALLDIR)/$(LIBDIR64)
	cp -af lib64/libvhsum.a $(INSTALLDIR)/$(LIBDIR64)
	mkdir -p $(INSTALLDIR)/$(LIBDIR32)
	cp -af lib32/libvhsum.a $(INSTALLDIR)/$(LIBDIR32) 2> /dev/null || :
	mkdir -p $(INSTALLDIR)/include
	cp -af src/vectorhash.h $(INSTALLDIR)/include
	mkdir -p $(INSTALLDIR)/man/man1
	cp -af man/vh32sum.1 $(INSTALLDIR)/man/man1
	cd $(INSTALLDIR)/man/man1; \
	$(GZIP) vh32sum.1; \
	ln -s vh32sum.1$(GZEXT) vh64sum.1$(GZEXT); \
	ln -s vh32sum.1$(GZEXT) vh128sum.1$(GZEXT); \
	ln -s vh32sum.1$(GZEXT) vh256sum.1$(GZEXT); \
	ln -s vh32sum.1$(GZEXT) vh512sum.1$(GZEXT); \
	ln -s vh32sum.1$(GZEXT) vh1024sum.1$(GZEXT)
	mkdir -p $(INSTALLDIR)/man/man3
	cp -af man/VectorHash.3 $(INSTALLDIR)/man/man3
	cd $(INSTALLDIR)/man/man3; \
	$(GZIP) VectorHash.3

ifneq ($(DEP_GOALS),)
include Makefile.dep
endif

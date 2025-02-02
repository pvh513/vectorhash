CXX = g++
CXXFLAGS = -g -W -Wall -Wno-unused-command-line-argument -std=c++11 -O3 -funroll-loops
LDFLAGS = -lvhsum -L.

INSTALLDIR = /usr/local
LIBDIR = lib64

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


.PHONY: all default testclean clean distclean install check

all: default

default: vh64sum vh128sum vh256sum vh512sum vh1024sum libvhsum.a

testclean:
	cd tests; \
	$(MAKE) clean

clean: testclean
	rm -f src/*.o
	rm -f vh128sum
	rm -f vh256sum
	rm -f vh512sum
	rm -f vh1024sum
	rm -f libvhsum.a

distclean: clean
	cd cpuid; \
	$(MAKE) clean
	cd unittest-cpp; \
	$(MAKE) clean

vh64sum: src/vectorhash.o cpuid/libcpuid.a libvhsum.a
	$(CXX) src/vectorhash.o $(LDFLAGS) -o $@

vh128sum: vh64sum
	ln -f vh64sum vh128sum

vh256sum: vh64sum
	ln -f vh64sum vh256sum

vh512sum: vh64sum
	ln -f vh64sum vh512sum

vh1024sum: vh64sum
	ln -f vh64sum vh1024sum

src/vectorhash.o: src/vectorhash.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_avx512.h \
	src/vectorhash_avx2.h src/vectorhash_sse2.h src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c src/vectorhash.cc -o $@

src/vectorhash_core.o: src/vectorhash_core.cc src/vectorhash.h src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx512.h src/vectorhash_avx2.h src/vectorhash_sse2.h src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c src/vectorhash_core.cc -o $@

src/vectorhash_finalize_64.o: src/vectorhash_finalize.cc src/vectorhash_priv.h src/vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH64 src/vectorhash_finalize.cc -o $@

src/vectorhash_finalize_128.o: src/vectorhash_finalize.cc src/vectorhash_priv.h src/vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH128 src/vectorhash_finalize.cc -o $@

src/vectorhash_finalize_256.o: src/vectorhash_finalize.cc src/vectorhash_priv.h src/vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH256 src/vectorhash_finalize.cc -o $@

src/vectorhash_finalize_512.o: src/vectorhash_finalize.cc src/vectorhash_priv.h src/vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH512 src/vectorhash_finalize.cc -o $@

src/vectorhash_finalize_1024.o: src/vectorhash_finalize.cc src/vectorhash_priv.h src/vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 src/vectorhash_finalize.cc -o $@

src/vectorhash_avx512_64.o: src/vectorhash_avx512.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH64 -mavx512f src/vectorhash_avx512.cc -o $@

src/vectorhash_avx512_128.o: src/vectorhash_avx512.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH128 -mavx512f src/vectorhash_avx512.cc -o $@

src/vectorhash_avx512_256.o: src/vectorhash_avx512.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH256 -mavx512f src/vectorhash_avx512.cc -o $@

src/vectorhash_avx512_512.o: src/vectorhash_avx512.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH512 -mavx512f src/vectorhash_avx512.cc -o $@

src/vectorhash_avx512_1024.o: src/vectorhash_avx512.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 -mavx512f src/vectorhash_avx512.cc -o $@

src/vectorhash_avx2_64.o: src/vectorhash_avx2.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH64 -mavx2 src/vectorhash_avx2.cc -o $@

src/vectorhash_avx2_128.o: src/vectorhash_avx2.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH128 -mavx2 src/vectorhash_avx2.cc -o $@

src/vectorhash_avx2_256.o: src/vectorhash_avx2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH256 -mavx2 src/vectorhash_avx2.cc -o $@

src/vectorhash_avx2_512.o: src/vectorhash_avx2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH512 -mavx2 src/vectorhash_avx2.cc -o $@

src/vectorhash_avx2_1024.o: src/vectorhash_avx2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 -mavx2 src/vectorhash_avx2.cc -o $@

src/vectorhash_sse2_64.o: src/vectorhash_sse2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH64 -msse2 src/vectorhash_sse2.cc -o $@

src/vectorhash_sse2_128.o: src/vectorhash_sse2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH128 -msse2 src/vectorhash_sse2.cc -o $@

src/vectorhash_sse2_256.o: src/vectorhash_sse2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH256 -msse2 src/vectorhash_sse2.cc -o $@

src/vectorhash_sse2_512.o: src/vectorhash_sse2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH512 -msse2 src/vectorhash_sse2.cc -o $@

src/vectorhash_sse2_1024.o: src/vectorhash_sse2.cc src/vectorhash_priv.h src/vectorhash_core.h src/vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 -msse2 src/vectorhash_sse2.cc -o $@

src/vectorhash_scalar_64.o: src/vectorhash_scalar.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH64 src/vectorhash_scalar.cc -o $@

src/vectorhash_scalar_128.o: src/vectorhash_scalar.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH128 src/vectorhash_scalar.cc -o $@

src/vectorhash_scalar_256.o: src/vectorhash_scalar.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH256 src/vectorhash_scalar.cc -o $@

src/vectorhash_scalar_512.o: src/vectorhash_scalar.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH512 src/vectorhash_scalar.cc -o $@

src/vectorhash_scalar_1024.o: src/vectorhash_scalar.cc src/vectorhash_priv.h src/vectorhash_core.h \
	src/vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 src/vectorhash_scalar.cc -o $@

cpuid/libcpuid.a:
	cd cpuid; \
	$(MAKE)

unittest-cpp/libUnitTest++.a:
	cd unittest-cpp; \
	$(MAKE)

libvhsum.a: src/vectorhash_core.o src/vectorhash_finalize_64.o src/vectorhash_finalize_128.o \
	src/vectorhash_finalize_256.o src/vectorhash_finalize_512.o src/vectorhash_finalize_1024.o \
	src/vectorhash_scalar_64.o src/vectorhash_scalar_128.o src/vectorhash_scalar_256.o \
	src/vectorhash_scalar_512.o src/vectorhash_scalar_1024.o src/vectorhash_sse2_64.o \
	src/vectorhash_sse2_128.o src/vectorhash_sse2_256.o src/vectorhash_sse2_512.o src/vectorhash_sse2_1024.o \
	src/vectorhash_avx2_64.o src/vectorhash_avx2_128.o src/vectorhash_avx2_256.o src/vectorhash_avx2_512.o \
	src/vectorhash_avx2_1024.o src/vectorhash_avx512_64.o src/vectorhash_avx512_128.o src/vectorhash_avx512_256.o \
	src/vectorhash_avx512_512.o src/vectorhash_avx512_1024.o cpuid/cpuinfo.o cpuid/version.o
	ar cr libvhsum.a $^
	${RANLIB} libvhsum.a

check: cpuid/libcpuid.a unittest-cpp/libUnitTest++.a libvhsum.a
	cd tests; \
	$(MAKE)

install: vh64sum vh128sum vh256sum vh512sum vh1024sum libvhsum.a
	mkdir -p $(INSTALLDIR)/bin
	cp -af vh64sum vh128sum vh256sum vh512sum vh1024sum $(INSTALLDIR)/bin
	mkdir -p $(INSTALLDIR)/$(LIBDIR)
	cp -af libvhsum.a $(INSTALLDIR)/$(LIBDIR)
	mkdir -p $(INSTALLDIR)/include
	cp -af src/vectorhash.h $(INSTALLDIR)/include
	mkdir -p $(INSTALLDIR)/man/man1
	cp -af man/vh128sum.1 $(INSTALLDIR)/man/man1
	cd $(INSTALLDIR)/man/man1; \
	$(GZIP) vh128sum.1; \
	ln -s vh128sum.1$(GZEXT) vh256sum.1$(GZEXT); \
	ln -s vh128sum.1$(GZEXT) vh512sum.1$(GZEXT); \
	ln -s vh128sum.1$(GZEXT) vh1024sum.1$(GZEXT)
	mkdir -p $(INSTALLDIR)/man/man3
	cp -af man/VectorHash.3 $(INSTALLDIR)/man/man3
	cd $(INSTALLDIR)/man/man3; \
	$(GZIP) VectorHash.3

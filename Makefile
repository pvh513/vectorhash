CXX = g++
CXXFLAGS = -g -W -Wall -std=c++11 -O3 -funroll-loops
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


.PHONY: all default clean veryclean distclean install check

all: default

default: vh128sum vh256sum vh512sum vh1024sum libvhsum.a

clean:
	rm -f *.o

veryclean: clean
	rm -f vh128sum
	rm -f vh256sum
	rm -f vh512sum
	rm -f vh1024sum
	rm -f libvhsum.a

distclean: veryclean
	cd cpuid; \
	$(MAKE) clean
	cd unittest-cpp; \
	$(MAKE) clean
	cd tests; \
	$(MAKE) clean

vh128sum: vectorhash.o cpuid/libcpuid.a libvhsum.a
	$(CXX) vectorhash.o $(LDFLAGS) -o $@

vh256sum: vh128sum
	ln -f vh128sum vh256sum

vh512sum: vh128sum
	ln -f vh128sum vh512sum

vh1024sum: vh128sum
	ln -f vh128sum vh1024sum

vectorhash.o: vectorhash.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx512.h \
	vectorhash_avx2.h vectorhash_sse2.h vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c vectorhash.cc -o $@

vectorhash_core.o: vectorhash_core.cc vectorhash.h vectorhash_priv.h vectorhash_core.h \
	vectorhash_avx512.h vectorhash_avx2.h vectorhash_sse2.h vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c vectorhash_core.cc -o $@

vectorhash_finalize_128.o: vectorhash_finalize.cc vectorhash_priv.h vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH128 vectorhash_finalize.cc -o $@

vectorhash_finalize_256.o: vectorhash_finalize.cc vectorhash_priv.h vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH256 vectorhash_finalize.cc -o $@

vectorhash_finalize_512.o: vectorhash_finalize.cc vectorhash_priv.h vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH512 vectorhash_finalize.cc -o $@

vectorhash_finalize_1024.o: vectorhash_finalize.cc vectorhash_priv.h vectorhash_core.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 vectorhash_finalize.cc -o $@

vectorhash_avx512_128.o: vectorhash_avx512.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH128 -mavx512f vectorhash_avx512.cc -o $@

vectorhash_avx512_256.o: vectorhash_avx512.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH256 -mavx512f vectorhash_avx512.cc -o $@

vectorhash_avx512_512.o: vectorhash_avx512.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH512 -mavx512f vectorhash_avx512.cc -o $@

vectorhash_avx512_1024.o: vectorhash_avx512.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx512.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 -mavx512f vectorhash_avx512.cc -o $@

vectorhash_avx2_128.o: vectorhash_avx2.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH128 -mavx2 vectorhash_avx2.cc -o $@

vectorhash_avx2_256.o: vectorhash_avx2.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH256 -mavx2 vectorhash_avx2.cc -o $@

vectorhash_avx2_512.o: vectorhash_avx2.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH512 -mavx2 vectorhash_avx2.cc -o $@

vectorhash_avx2_1024.o: vectorhash_avx2.cc vectorhash_priv.h vectorhash_core.h vectorhash_avx2.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 -mavx2 vectorhash_avx2.cc -o $@

vectorhash_sse2_128.o: vectorhash_sse2.cc vectorhash_priv.h vectorhash_core.h vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH128 -msse2 vectorhash_sse2.cc -o $@

vectorhash_sse2_256.o: vectorhash_sse2.cc vectorhash_priv.h vectorhash_core.h vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH256 -msse2 vectorhash_sse2.cc -o $@

vectorhash_sse2_512.o: vectorhash_sse2.cc vectorhash_priv.h vectorhash_core.h vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH512 -msse2 vectorhash_sse2.cc -o $@

vectorhash_sse2_1024.o: vectorhash_sse2.cc vectorhash_priv.h vectorhash_core.h vectorhash_sse2.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 -msse2 vectorhash_sse2.cc -o $@

vectorhash_scalar_128.o: vectorhash_scalar.cc vectorhash_priv.h vectorhash_core.h vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH128 vectorhash_scalar.cc -o $@

vectorhash_scalar_256.o: vectorhash_scalar.cc vectorhash_priv.h vectorhash_core.h vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH256 vectorhash_scalar.cc -o $@

vectorhash_scalar_512.o: vectorhash_scalar.cc vectorhash_priv.h vectorhash_core.h vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH512 vectorhash_scalar.cc -o $@

vectorhash_scalar_1024.o: vectorhash_scalar.cc vectorhash_priv.h vectorhash_core.h vectorhash_scalar.h
	$(CXX) $(CXXFLAGS) -c -DVH1024 vectorhash_scalar.cc -o $@

cpuid/libcpuid.a:
	cd cpuid; \
	$(MAKE)

unittest-cpp/libUnitTest++.a:
	cd unittest-cpp; \
	$(MAKE)

libvhsum.a: vectorhash_core.o vectorhash_finalize_128.o vectorhash_finalize_256.o \
	vectorhash_finalize_512.o vectorhash_finalize_1024.o vectorhash_scalar_128.o vectorhash_scalar_256.o \
	vectorhash_scalar_512.o vectorhash_scalar_1024.o vectorhash_sse2_128.o vectorhash_sse2_256.o \
	vectorhash_sse2_512.o vectorhash_sse2_1024.o vectorhash_avx2_128.o vectorhash_avx2_256.o \
	vectorhash_avx2_512.o vectorhash_avx2_1024.o vectorhash_avx512_128.o vectorhash_avx512_256.o \
	vectorhash_avx512_512.o vectorhash_avx512_1024.o cpuid/cpuinfo.o cpuid/version.o
	ar cr libvhsum.a $^
	${RANLIB} libvhsum.a

check: cpuid/libcpuid.a unittest-cpp/libUnitTest++.a libvhsum.a
	cd tests; \
	$(MAKE)

install: vh128sum vh256sum vh512sum vh1024sum libvhsum.a
	mkdir -p $(INSTALLDIR)/bin
	cp -af vh128sum vh256sum vh512sum vh1024sum $(INSTALLDIR)/bin
	mkdir -p $(INSTALLDIR)/$(LIBDIR)
	cp -af libvhsum.a $(INSTALLDIR)/$(LIBDIR)
	mkdir -p $(INSTALLDIR)/include
	cp -af vectorhash.h $(INSTALLDIR)/include
	mkdir -p $(INSTALLDIR)/man/man1
	cp -af vh128sum.1 $(INSTALLDIR)/man/man1
	cd $(INSTALLDIR)/man/man1; \
	$(GZIP) vh128sum.1; \
	ln -s vh128sum.1$(GZEXT) vh256sum.1$(GZEXT); \
	ln -s vh128sum.1$(GZEXT) vh512sum.1$(GZEXT); \
	ln -s vh128sum.1$(GZEXT) vh1024sum.1$(GZEXT)
	mkdir -p $(INSTALLDIR)/man/man3
	cp -af VectorHash.3 $(INSTALLDIR)/man/man3
	cd $(INSTALLDIR)/man/man3; \
	$(GZIP) VectorHash.3

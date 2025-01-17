CXX = g++
CXXFLAGS = -g -W -Wall -std=c++11 -O3

RANLIB := ${shell which ranlib}
ifneq ($(RANLIB),)
  RANLIB = ranlib
else
  RANLIB = ar ts
endif

.PHONY: all default clean

all: default

default: libcpuid.a

clean:
	rm -f *.o
	rm -f libcpuid.a

libcpuid.a: cpuinfo.o version.o
	ar cr libcpuid.a $^
	$(RANLIB) libcpuid.a

cpuinfo.o: cpuinfo.cpp cpuinfo.hpp detail/cpuinfo_impl.hpp detail/init_apple_gcc_arm.hpp \
	detail/init_linux_gcc_arm.hpp detail/init_msvc_x86.hpp detail/extract_x86_flags.hpp \
	detail/init_gcc_x86.hpp detail/init_msvc_arm.hpp detail/init_unknown.hpp platform/config.hpp
	$(CXX) $(CXXFLAGS) -c cpuinfo.cpp

version.o: version.cpp version.hpp
	$(CXX) $(CXXFLAGS) -c version.cpp

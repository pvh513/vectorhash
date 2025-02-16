.PHONY: all check64 check32 clean

CXX = g++
CXXFLAGS = -g -W -Wall -ansi -std=c++11 -O0 -I../src -I../cpuid -I../unittest-cpp
LDFLAGS = -lUnitTest++ -lvhsum

ifeq ($(MAKECMDGOALS),check32)
  CXXFLAGS += -m32
  LDFLAGS += -L../unittest-cpp/lib32 -L../lib32 -m32
else
  LDFLAGS += -L../unittest-cpp/lib64 -L../lib64
endif

all: check64

check64: TestMain run_tests.sh
	@echo "=============================="
	@echo " Starting VectorHash tests... "
	@echo "=============================="
	./TestMain
	./run_tests.sh

check32: TestMain
	@echo "=============================="
	@echo " Starting VectorHash tests... "
	@echo "=============================="
	./TestMain

clean:
	rm -f *.o
	rm -f TestMain

%.o: %.cpp TestMain.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

TestMain: TestMain.o TestScalar.o TestSSE2.o TestAVX2.o TestAVX512f.o
	$(CXX) $^ $(LDFLAGS) -o TestMain

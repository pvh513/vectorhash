CXX = g++
CXXFLAGS = -g -O0 -W -Wall -I../src -I../cpuid -I../unittest-cpp
LDFLAGS = -lcpuid -L../cpuid -lUnitTest++ -L../unittest-cpp -lvhsum -L..

.PHONY: all clean

all: TestMain

clean:
	rm -f *.o
	rm -f TestMain

%.o: %.cpp TestMain.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

TestMain: TestMain.o TestScalar.o TestSSE2.o TestAVX2.o TestAVX512f.o
	$(CXX) $^ $(LDFLAGS) -o TestMain
	@echo "=============================="
	@echo " Starting VectorHash tests... "
	@echo "=============================="
	./TestMain

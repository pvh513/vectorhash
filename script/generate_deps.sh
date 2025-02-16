#!/bin/sh

make_deps () {
	out=`echo $2 | sed s/:.*//`
	if [ "$1" != "src/vectorhash.cc" ]; then
		counter="${counter}="
		if [ "$counter" == "===" ]; then
			lib64="${lib64} \\\\\\n"
			lib32="${lib32} \\\\\\n"
			counter=""
		fi
		lib64="${lib64} lib64/$out"
		lib32="${lib32} lib32/$out"
	fi
	echo "lib64/$2"
	echo -e "\t\$(CXX) \$(CXXFLAGS) -c $1 -o \$@"
	echo
	echo "lib32/$2"
	echo -e "\t\$(CXX) \$(CXXFLAGS) -c -m32 $1 -o \$@"
	echo
}

make_deps_sub () {
	flag=""
	case "$2" in
		*sse2*) flag="-msse2" ;;
		*avx2*) flag="-mavx2" ;;
		*avx512*) flag="-mavx512f" ;;
	esac
	out=`echo $3 | sed s/:.*//`
	counter="${counter}="
	if [ "$counter" == "===" ]; then
		lib64="${lib64} \\\\\\n"
		lib32="${lib32} \\\\\\n"
		counter=""
	fi
	lib64="${lib64} lib64/$1/$out"
	lib32="${lib32} lib32/$1/$out"
	echo "lib64/$1/$3"
	echo -e "\t\$(CXX) \$(CXXFLAGS) -c -D$1 $flag $2 -o \$@"
	echo
	echo "lib32/$1/$3"
	echo -e "\t\$(CXX) \$(CXXFLAGS) -c -m32 -D$1 $flag $2 -o \$@"
	echo
}

cxx=$1
cxxflags=$2

lib64="lib64/libvhsum.a:"
lib32="lib32/libvhsum.a:"

counter=""

for file in src/*.cc
do
	multibuild=`grep -l 'EXT(' $file`
	deps=`$cxx $cxxflags -MM $file`
	if [ -n "$multibuild" ]; then
		make_deps_sub "VH32" "$file" "$deps"
		make_deps_sub "VH64" "$file" "$deps"
		make_deps_sub "VH128" "$file" "$deps"
		make_deps_sub "VH256" "$file" "$deps"
		make_deps_sub "VH512" "$file" "$deps"
		make_deps_sub "VH1024" "$file" "$deps"
	else
		make_deps "$file" "$deps"
	fi
done
for file in cpuid/*.cpp
do
	deps=`$cxx $cxxflags -MM $file`
	make_deps "$file" "$deps"
done
echo -e "$lib64"
echo -e "\tar cr lib64/libvhsum.a \$^"
echo -e "\t\$(RANLIB) lib64/libvhsum.a"
echo
echo -e "$lib32"
echo -e "\tar cr lib32/libvhsum.a \$^"
echo -e "\t\$(RANLIB) lib32/libvhsum.a"

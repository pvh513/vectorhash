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

ver=1

OS=`uname -s`

if [ "${OS}" = "Darwin" ] ; then
	ext="dylib"
	nm64="libvhsum.${ver}.${ext}"
	nm32="libvhsum.${ver}.${ext}"
	libflags="-dynamiclib -install_name @rpath/libvhsum.${ext}"
else
	ext="so"
	nm64="libvhsum.${ext}.${ver}"
	nm32="libvhsum.${ext}.${ver}"
	libflags="-shared"
fi

lib64="lib64/${nm64}:"
lib32="lib32/${nm32}:"

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
echo -e "\t\$(CXX) \$(CXXFLAGS) $libflags -o lib64/$nm64 \$^"
echo -e "\tln -sf $nm64 lib64/libvhsum.${ext}"
echo
echo -e "$lib32"
echo -e "\t\$(CXX) \$(CXXFLAGS) -m32 $libflags -o lib32/$nm32 \$^"
echo -e "\tln -sf $nm32 lib32/libvhsum.${ext}"
echo
echo -e "lib64: lib64/$nm64"
echo
echo -e "lib32: lib32/$nm32"
echo
echo -e "install-lib:"
echo -e "\tmkdir -p \$(INSTALLDIR)/\$(LIBDIR64)"
echo -e "\tstrip -x lib64/$nm64"
echo -e "\tcp -af lib64/libvhsum.* \$(INSTALLDIR)/\$(LIBDIR64)"
echo -e "\tmkdir -p \$(INSTALLDIR)/\$(LIBDIR32)"
echo -e "\tstrip -x lib32/$nm32 2> /dev/null || :"
echo -e "\tcp -af lib32/libvhsum.* \$(INSTALLDIR)/\$(LIBDIR32) 2> /dev/null || :"
echo
if [ "${OS}" = "Darwin" ] ; then
	echo -e "LDFLAGS = -lvhsum -Llib64 -Wl,-rpath=\$(INSTALLDIR)/\$(LIBDIR64)"
else
	echo -e "LDFLAGS = -l:$nm64 -Llib64"
fi

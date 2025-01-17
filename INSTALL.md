## Vectorhash - a very fast vectorized hash function

### Compiling the code

A configuration step is not needed, so we can go straight to the actual
compilation. A GNU Makefile is supplied, so you can do the compilation simply by
typing:

	make

On some operating systems you may have to use <tt> gmake</tt> instead of
<tt>make</tt>. If you want to do a parallel build, e.g. using 16 cores, you can type:

	make -j16

The build will create four executables called vh128sum, vh256sum, vh512sum, and
vh1024sum. These are hard-linked to each other, so they really only take up the
space of one executable. The Makefile is set up to use the GNU compiler g++. It
is straightforward to use the LLVM compiler clang++ instead:

	make -j16 CXX=clang++

In principle it should be possible to use other C++11 compatible compilers too, but
this has not been tested. In that case you will most likely have to change the compiler
options as well. Assuming your favorite compiler is called <tt>CC</tt>, you would get
something like this:

	make -j16 CXX=CC CXXFLAGS="CC-specific-flags"

The build process will also create a static 64-bit library called <tt>libvhsum.a</tt>.
This can be used to checksum buffers in your own programs. See the file
<tt>README.md</tt> for more details.

### Installing the code

Installing the code can be done by simply typing:

	make install

You may need root privileges to install the files in the location you have chosen.
In that case you will need to log in as root or use the <tt>sudo</tt> command:

	sudo make install

By default the installation will be done in <tt>/usr/local</tt>. If you want to use a
different directory, you can type:

	make install INSTALLDIR=/path/to/dir

This will install files in <tt>INSTALLDIR/bin</tt> (for the executables),
<tt>INSTALLDIR/lib64</tt> (for the library), <tt>INSTALLDIR/include</tt> (for the
header file), and <tt>INSTALLDIR/man/man*</tt> (for the man pages). So the
default location for installing the library is <tt>/usr/local/lib64</tt>. This may not
be the appropriate location for 64-bit libraries on all operating systems. For
instance on Debian-based Linux distros, they would need to be installed in
<tt>/usr/local/lib</tt>. You can achieve this with the following command:

	make install LIBDIR=lib

Peter A.M. van Hoof   
Royal Observatory of Belgium   
p.vanhoof@oma.be

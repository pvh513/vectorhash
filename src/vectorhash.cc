//-------------------------------------------------------------------------------
//  VectorHash - a very fast hash function optimized using SIMD instructions
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <vector>

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#else
#define _POSIX_MAPPED_FILES 0
#endif

#if _POSIX_MAPPED_FILES > 0
#include <sys/mman.h>
#endif

#include <fcntl.h>

// O_BINARY is not defined on systems where there
// is no distinction between binary and text I/O.
#ifndef O_BINARY
#define O_BINARY 0
#endif

#include "vectorhash_priv.h"
#include "vectorhash_core.h"
#include "vectorhash_finalize.h"
#include "vectorhash_avx512.h"
#include "vectorhash_avx2.h"
#include "vectorhash_sse2.h"
#include "vectorhash_scalar.h"

static string SIMDname[] = { "Scalar", "SSE2", "AVX2", "AVX512" };

struct vh_params {
	string cmd;
	string name;
	bool lgBSDstyle;
	bool lgCheckMode;
	bool lgIgnoreMissing;
	bool lgBinarySet;
	bool lgTextSet;
	bool lgBinary;
	bool lgQuiet;
	bool lgStatusOnly;
	bool lgStrict;
	bool lgWarnSyntax;
	bool lgVerbose;
	bool lgZero;
	is_type SIMDversion;
	int returncode;
	uint32_t seed;
	size_t vh_hash_width;
	size_t vh_virtreg_width;
	size_t vh_nstate;
	size_t vh_nint;
	size_t blocksize;
	bool set_hash_width(size_t hw)
	{
		// width of the hash (in bits)
		// this value MUST be a power of 2 between 32 and 1024
		if( hw < 32 || hw > 1024 || ((hw & (hw-1)) != 0) )
			return false;
		vh_hash_width = hw;
		// width of the virtual SIMD register supported in VectorHash (in bits)
		// must be at least as wide as the largest hardware register that is used
		vh_virtreg_width = ( 2*vh_hash_width > vh_hwreg_width ) ? 2*vh_hash_width : vh_hwreg_width;
		// width of the hash in uint32_t elements
		vh_nstate = vh_hash_width/32;
		// number of uint32_t's that fit in the virtual register
		vh_nint = vh_virtreg_width/32;
		// the file is read with this blocksize (in bytes)
		blocksize = 4*vh_nint*sizeof(uint32_t);
		// update the name as well...
		ostringstream oss;
		oss << "VH" << vh_hash_width;
		name = oss.str();
		return true;
	}
	vh_params() : lgBSDstyle(false), lgCheckMode(false), lgIgnoreMissing(false), lgBinarySet(false),
				  lgTextSet(false), lgBinary(false), lgQuiet(false), lgStatusOnly(false), lgStrict(false),
				  lgWarnSyntax(false), lgVerbose(false), lgZero(false), SIMDversion(IS_INVALID),
				  returncode(0), seed(0xfd4c799d)
	{
		(void)set_hash_width(32);
	}
	char sentinel() const { return lgBinary ? '*' : ' '; }
	string option() const { return lgBinary ? "rb" : "r"; }
	void SetSIMDVersion()
	{
		// check if SIMD version was already forced by a command line option
		if( SIMDversion == IS_INVALID )
		{
			SIMDversion = GetSIMDVersion();
			if( lgVerbose )
			{
				cout << "found SIMD capability: " << SIMDname[SIMDversion] << "." << endl;
			}
		}
		else {
			if( lgVerbose )
			{
				cout << "SIMD capability was set on the command line to: " << SIMDname[SIMDversion] << "." << endl;
			}
		}			
	}
};

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#if defined(__CYGWIN__)
// _aligned_malloc / _aligned_free defined in Windows, but not Cygwin, reported by Richard Rudy
inline int posix_memalign(void **p, size_t a, size_t s)
{
	*p = aligned_alloc(s, a);
	return ( *p == NULL ) ? errno : 0;
}

inline void posix_memalign_free(void *p)
{
	free(p);
}
#elif defined(_MSC_VER)
// posix_memalign not defined on windows
inline int posix_memalign(void **p, size_t a, size_t s)
{
	*p = _aligned_malloc(s, a);
	return ( *p == NULL ) ? errno : 0;
}

inline void posix_memalign_free(void *p)
{
	_aligned_free(p);
}
#else
inline void posix_memalign_free(void *p)
{
	free(p);
}
#endif

inline string escfn(const string& s)
{
	// TODO this routine is too simplistic and needs more work...
	bool lgEsc = ( s.length() == 0 );
	for( auto c : s )
		if( isspace(c) || c == '\\' )
			lgEsc = true;
	if( lgEsc )
		return "\'" + s + "\'";
	else
		return s;
}

static string VHstream(const vh_params& vhp, FILE* io)
{
	if( fseek( io, 0, SEEK_END ) != 0 )
		return string();
	long fsize = ftell(io);
	if( fsize < 0 )
		return string();
	vector<uint32_t> state(vhp.vh_nstate);
#if _POSIX_MAPPED_FILES > 0
	int fd = fileno(io);
	char* map = ( fsize > 0 ) ? (char*)mmap( NULL, fsize, PROT_READ, MAP_SHARED, fd, 0 ) : nullptr;
	if( fsize > 0 && map == MAP_FAILED )
		return string();
	VectorHash( map, fsize, vhp.seed, state.data(), vhp.SIMDversion, vhp.vh_hash_width );
	munmap(map, fsize);
#else
	if( fseek( io, 0, SEEK_SET ) != 0 )
		return string();
	void* map = NULL;
	if( fsize > 0 )
	{
		if( posix_memalign( &map, vh_hwreg_width/8, fsize ) != 0 )
			return string();
		if( fread( map, fsize, 1, io ) != 1 )
			return string();
	}
	VectorHash( map, fsize, vhp.seed, state.data(), vhp.SIMDversion, vhp.vh_hash_width );
	if( map != NULL )
		posix_memalign_free( map );
#endif

	ostringstream hash;
	for( uint32_t i=0; i < vhp.vh_nstate; ++i )
		hash << hex << setfill('0') << setw(8) << state[i];

	return hash.str();
}

static string VHstdin(const vh_params& vhp)
{
	void* h1;
	if( posix_memalign( &h1, vh_hwreg_width/8, vhp.vh_nint*sizeof(uint32_t) ) != 0 )
		return string();
	void* h2;
	if( posix_memalign( &h2, vh_hwreg_width/8, vhp.vh_nint*sizeof(uint32_t) ) != 0 )
		return string();
	void* h3;
	if( posix_memalign( &h3, vh_hwreg_width/8, vhp.vh_nint*sizeof(uint32_t) ) != 0 )
		return string();
	void* h4;
	if( posix_memalign( &h4, vh_hwreg_width/8, vhp.vh_nint*sizeof(uint32_t) ) != 0 )
		return string();

	uint32_t* z1 = (uint32_t*)h1;
	uint32_t* z2 = (uint32_t*)h2;
	uint32_t* z3 = (uint32_t*)h3;
	uint32_t* z4 = (uint32_t*)h4;

	uint32_t seed = vhp.seed;
	stateinit( z1, seed, vhp.vh_nint );
	stateinit( z2, seed, vhp.vh_nint );
	stateinit( z3, seed, vhp.vh_nint );
	stateinit( z4, seed, vhp.vh_nint );

	void* map = NULL;
	if( posix_memalign( &map, vh_hwreg_width/8, vhp.blocksize ) != 0 )
		return string();
	vector<uint32_t> state(vhp.vh_nstate);
	size_t len = 0, bsize;
	bool lgContinue = true;
	while( lgContinue )
	{
		bsize = fread( map, 1, vhp.blocksize, stdin );
		if( bsize < vhp.blocksize ) {
			pad_buffer( (const uint8_t*)map, (uint8_t*)map, bsize, vhp.blocksize );
			lgContinue = false;
		}
		if( vhp.SIMDversion == IS_AVX512 )
			VectorHashBody512((const v16si*)map, (v16si*)h1, (v16si*)h2, (v16si*)h3, (v16si*)h4, vhp.vh_hash_width);
		else if( vhp.SIMDversion == IS_AVX2 )
			VectorHashBody256((const v8si*)map, (v8si*)h1, (v8si*)h2, (v8si*)h3, (v8si*)h4, vhp.vh_hash_width);
		else if( vhp.SIMDversion == IS_SSE2 )
			VectorHashBody128((const v4si*)map, (v4si*)h1, (v4si*)h2, (v4si*)h3, (v4si*)h4, vhp.vh_hash_width);
		else if( vhp.SIMDversion == IS_SCALAR )
			VectorHashBody32((const uint32_t*)map, (uint32_t*)h1, (uint32_t*)h2, (uint32_t*)h3, (uint32_t*)h4,
							 vhp.vh_hash_width);
		else {
			cout << "Internal error: impossible value for SIMD version: " << vhp.SIMDversion << "." << endl;
			exit(1);
		}
		len += bsize;
	}
	posix_memalign_free( map );

	if( vhp.vh_hash_width == 32 )
		VectorHashFinalize_32(len, z1, z2, z3, z4, state.data());
	else if( vhp.vh_hash_width == 64 )
		VectorHashFinalize_64(len, z1, z2, z3, z4, state.data());
	else if( vhp.vh_hash_width == 128 )
		VectorHashFinalize_128(len, z1, z2, z3, z4, state.data());
	else if( vhp.vh_hash_width == 256 )
		VectorHashFinalize_256(len, z1, z2, z3, z4, state.data());
	else if( vhp.vh_hash_width == 512 )
		VectorHashFinalize_512(len, z1, z2, z3, z4, state.data());
	else if( vhp.vh_hash_width == 1024 )
		VectorHashFinalize_1024(len, z1, z2, z3, z4, state.data());
	else {
		cout << "Internal error: impossible value for hash width: " << vhp.vh_hash_width << "." << endl;
		exit(1);
	}

	posix_memalign_free( h4 );
	posix_memalign_free( h3 );
	posix_memalign_free( h2 );
	posix_memalign_free( h1 );

	ostringstream hash;
	for( uint32_t i=0; i < vhp.vh_nstate; ++i )
		hash << hex << setfill('0') << setw(8) << state[i];

	return hash.str();
}

inline string Escape(const string& s)
{
	string t;
	for( size_t p=0; p < s.length(); p++ )
	{
		if( s[p] == '\\' )
			t += "\\\\";
		else if( s[p] == '\n' )
			t += "\\n";
		else
			t += s[p];
	}
	return t;
}

inline string DeEscape(const string& s)
{
	string t;
	for( size_t p=0; p < s.length(); )
	{
		if( s[p] == '\\' )
		{
			if( s.substr(p, 2) == "\\n" )
			{
				t += "\n";
				p += 2;
			}
			else if( s.substr(p, 2) == "\\\\" )
			{
				t += "\\";
				p += 2;
			}
			else
			{
				// this is a syntax error...
				return string();
			}
		}
		else
			t += s[p++];
	}
	return t;
}

inline void PrintSum(const vh_params& vhp, const string& arg, const string& vhsum)
{
	string esc;
	if( vhp.lgZero )
		esc = arg;
	else {
		esc = Escape(arg);
		if( esc != arg )
			cout << '\\';
	}
	if( vhp.lgBSDstyle )
		cout << "VH" << vhp.vh_hash_width << " (" << esc << ") = " << vhsum;
	else
		cout << vhsum << " " << vhp.sentinel() << esc;
	cout << ( vhp.lgZero ? '\0' : '\n' );
}

static void PrintHelp(const vh_params& vhp)
{
	cout << "Usage: " << vhp.cmd << " [OPTION]... [FILE]...\n";
	cout << "Print vectorized hash (" << vhp.vh_hash_width << "-bit) checksums.\n";
	cout << endl;
	cout << "With no FILE, or when FILE is -, read standard input.\n";
	cout << endl;
	cout << "  -b, --binary          read FILE in binary mode\n";
	cout << "  -c, --check           read hashes of the FILEs and check them\n";
	cout << "      --tag             create BSD-style output\n";
	cout << "  -t, --text            read FILE in text mode (default)\n";
	cout << "  -z, --zero            end each output line with NUL, not newline,\n";
	cout << "                        and disable file name escaping\n";
	cout << "  -l, --length          set checksum width (allowed values 32 <= 2^n <= 1024)\n";
	cout << "      --                this flag terminates the list of OPTIONs, allowing FILE\n";
	cout << "                        names starting with \"-\" to be used after this flag\n"; 
	cout << endl;
	cout << "The following four options are mainly useful for testing:\n";
	cout << "      --scalar          force using scalar version of algorithm\n";
	cout << "      --sse2            force using SSE2 version of algorithm\n";
	cout << "      --avx2            force using AVX2 version of algorithm\n";
	cout << "      --avx512          force using AVX512f version of algorithm\n";
	cout << endl;
	cout << "The following five options are useful only when verifying checksums:\n";
	cout << "  -i, --ignore-missing  don't fail or report status for missing files\n";
	cout << "  -q, --quiet           don't print OK for each successfully verified file\n";
	cout << "  -Q, --status          don't output anything, status code shows success\n";
	cout << "  -s, --strict          exit non-zero for improperly formatted checksum lines\n";
	cout << "  -w, --warn            warn about improperly formatted checksum lines\n";
	cout << endl;
	cout << "  -h, --help            display this help and exit\n";
	cout << "  -v, --version         print version information and exit\n";
	cout << "      --verbose         print additional information (mainly for debugging)\n";
	cout << endl;
	cout << "Long versions of the OPTIONs can be abbreviated, provided the abbreviated form is\n";
	cout << "unambiguous.\n";
	cout << endl;
	cout << "The sums are computed using a vectorized algorithm that supports the AVX512f, AVX2,\n";
	cout << "and SSE2 instructions sets. A scalar version is also supplied for other hardware.\n";
	cout << "The resulting sum is identical for each of these instruction sets.\n";
	cout << endl;
	cout << "When checking, the input should be a former output of this program. The default mode\n";
	cout << "is to print a line with the checksum, a space, a character indicating the input mode\n";
	cout << "('*' for binary, ' ' for text), and the name for each FILE.\n";
	exit(0);
}

static void PrintVersion(const vh_params& vhp)
{
	cout << "vh" << vhp.vh_hash_width << "sum (vectorized hash, ";
	cout << vhp.vh_hash_width << "-bit) v" << vh_version << endl;
	cout << "Copyright (C) 2018-2025 Peter A.M. van Hoof.\n";
	cout << "License: the zlib/libpng license <https://opensource.org/licenses/Zlib>\n";
	cout << "This is open-source software and comes with no warranty.\n";
	exit(0);
}

static bool read_whole_line(string& chLine, FILE *io)
{
	chLine.clear();

	const size_t buflen = 200;
	char buf[buflen];
	char *chRet;
	while( (chRet = fgets( buf, buflen, io )) != 0 )
	{
		chLine += buf;
		char eos = ( chLine.length() > 0 ) ? chLine.back() : '\0';
		if( eos == '\n' )
			break;
	}
	return ( chLine.length() > 0 );
}

static void CheckFiles(vh_params& vhp, const string& arg, FILE* io)
{
	string line;
	size_t ioerror = 0;
	size_t failed = 0;
	size_t formaterr = 0;
	size_t correct = 0;
	size_t lineno= 0 ;
	// do not allow upper case hexadecimal digits as unmodified output should always be lower case
	const regex bsd_format( "^(\\\\)?VH([[:d:]]+) \\((.+)\\) = ([[:d:]a-f]+)$" );
	const regex std_format( "^(\\\\)?([[:d:]a-f]+) ([ *])(.+)$" );
	while( read_whole_line(line, io) )
	{
		while( true )
		{
			// erase trailing newline
			char c = line.back();
			if( c != '\n' && c != '\r' )
				break;
			line.pop_back();
		}
		++lineno;

		bool lgBinary, lgEscape;
		size_t hashlen = vhp.vh_hash_width/4;
		string vhsum1, path;
		smatch what;
		if( regex_match( line, what, bsd_format ) )
		{
			lgEscape = what[1].matched;
			size_t my_width;
			istringstream iss( what[2] );
			iss >> my_width;
			path = what[3];
			vhsum1 = what[4];
			lgBinary = true;
			if( my_width != vhp.vh_hash_width || vhsum1.length() != hashlen )
			{
				if( vhp.lgWarnSyntax )
				{
					cout << vhp.cmd << ": " << escfn(arg) << ": " << lineno;
					cout << ": improperly formatted " << vhp.name << " checksum line\n";
				}
				++formaterr;
				continue;
			}
			++correct;
		}
		else if( regex_match( line, what, std_format ) )
		{
			lgEscape = what[1].matched;
			vhsum1 = what[2];
			lgBinary = ( what[3] == "*" );
			path = what[4];
			if( vhsum1.length() != hashlen )
			{
				if( vhp.lgWarnSyntax )
				{
					cout << vhp.cmd << ": " << escfn(arg) << ": " << lineno;
					cout << ": improperly formatted " << vhp.name << " checksum line\n";
				}
				++formaterr;
				continue;
			}
			++correct;
		}
		else
		{
			if( vhp.lgWarnSyntax )
			{
				cout << vhp.cmd << ": " << escfn(arg) << ": " << lineno;
				cout << ": improperly formatted " << vhp.name << " checksum line\n";
			}
			++formaterr;
			continue;
		}
		if( lgEscape )
			path = DeEscape( path );
		string esc;
		if( path.find('\n') != string::npos )
			esc = "\\" + Escape( path );
		else
			esc = path;
		FILE* io = fopen( path.c_str(), ( lgBinary ? "rb" : "r" ) );
		string vhsum2;
		if( io == 0 )
		{
			if( !vhp.lgIgnoreMissing )
			{
				if( !vhp.lgStatusOnly )
					cout << vhp.cmd << ": " << escfn(path) << ": No such file or directory\n";
				vhp.returncode = 1;
			}
		}
		else
		{
			vhsum2 = VHstream( vhp, io );
			fclose( io );
		}
		if( vhsum2.length() == 0 && !vhp.lgIgnoreMissing )
		{
			if( !vhp.lgStatusOnly )
				cout << esc << ": FAILED open or read\n";
			vhp.returncode = 1;
			++ioerror;
		}
		if( vhsum1 == vhsum2 )
		{
			if( !vhp.lgQuiet && !vhp.lgStatusOnly )
				cout << esc << ": OK\n";
		}
		else if( vhsum2.length() == hashlen )
		{
			if( !vhp.lgStatusOnly )
				cout << esc << ": FAILED\n";
			vhp.returncode = 1;
			++failed;
		}
	}
	if( !vhp.lgStatusOnly )
	{
		if( ioerror == 1 )
			cout << vhp.cmd << ": WARNING: 1 listed file could not be read\n";
		else if( ioerror > 1 )
			cout << vhp.cmd << ": WARNING: " << ioerror << " listed files could not be read\n";
		if( failed == 1 )
			cout << vhp.cmd << ": WARNING: 1 computed checksum did NOT match\n";
		else if( failed > 1 )
			cout << vhp.cmd << ": WARNING: " << failed << " computed checksums did NOT match\n";
		if( correct == 0 ) {
			cout << vhp.cmd << ": " << escfn(arg) << ": no properly formatted " << vhp.name << " checksum lines found\n";
			vhp.returncode = 1;
		}
		else if( formaterr == 1 )
			cout << vhp.cmd << ": WARNING: 1 line is improperly formatted\n";
		else if( formaterr > 1 )
			cout << vhp.cmd << ": WARNING: " << formaterr << " lines are improperly formatted\n";
	}
	if( vhp.lgStrict && formaterr > 0 )
		vhp.returncode = 1;
}

static void ProcessFile(vh_params& vhp, const string& arg, FILE* io)
{
	if( vhp.lgVerbose )
	{
		cout << "blocksize: " << vhp.blocksize << " bytes, ";
		cout << "seed: 0x" << hex << setw(8) << setfill('0') << vhp.seed << endl;
	}
	if( vhp.lgCheckMode )
	{
		CheckFiles( vhp, arg, ( io == 0 ? stdin : io ) );
	}
	else
	{
		string vhsum = ( io == 0 ) ? VHstdin( vhp ) : VHstream( vhp, io );
		PrintSum( vhp, arg, vhsum );
	}
}

static void VerifyOptions( vh_params& vhp )
{
	if( vhp.lgBinarySet || vhp.lgBSDstyle )
		vhp.lgBinary = true;
	else if( vhp.lgTextSet )
		vhp.lgBinary = false;
	else
		vhp.lgBinary = O_BINARY;

	vhp.SetSIMDVersion();

	if( vhp.lgIgnoreMissing && !vhp.lgCheckMode )
	{
		cout << vhp.cmd << ": the --ignore-missing option is meaningful only when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgStatusOnly && !vhp.lgCheckMode )
	{
		cout << vhp.cmd << ": the --status option is meaningful only when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgQuiet && !vhp.lgCheckMode )
	{
		cout << vhp.cmd << ": the --quiet option is meaningful only when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgWarnSyntax && !vhp.lgCheckMode )
	{
		cout << vhp.cmd << ": the --warn option is meaningful only when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgStrict && !vhp.lgCheckMode )
	{
		cout << vhp.cmd << ": the --strict option is meaningful only when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgBSDstyle && vhp.lgCheckMode )
	{
		cout << vhp.cmd << ": the --tag option is meaningless when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgBSDstyle && vhp.lgTextSet )
	{
		cout << vhp.cmd << ": --tag does not support --text mode\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgCheckMode && ( vhp.lgBinarySet || vhp.lgTextSet ) )
	{
		cout << vhp.cmd << ": the --binary and --text options are meaningless when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgCheckMode && vhp.lgZero )
	{
		cout << vhp.cmd << ": the --zero option is not supported when verifying checksums\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
	if( vhp.lgStatusOnly && vhp.lgVerbose )
	{
		cout << vhp.cmd << ": the --verbose option conflicts with --status\n";
		cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
		exit(1);
	}
}

// the following struct and two routines was taken (and altered) from:
// http://www.geeksforgeeks.org/find-all-shortest-unique-prefixes-to-represent-each-word-in-a-given-list/
static const size_t TRIESZ = 128;

struct trieNode
{
	trieNode* child[TRIESZ];
	size_t freq;  // To store frequency
	trieNode()
	{
		memset(child, 0, TRIESZ*sizeof(trieNode*));
		freq = 0;
	}
	trieNode(const trieNode&) = delete;
	trieNode& operator= (const trieNode&) = delete;
	~trieNode()
	{
		for( size_t i=0; i < TRIESZ; i++ )
			delete child[i];
	}
};
 
// Method to insert a new string into Trie
void insertToken(trieNode* root, const string& token)
{
	trieNode* pCrawl = root;
 
	// Traversing over the length of given token.
	for( size_t level=0; level < token.length(); level++ )
	{
		// Get index of child node from current character in token.
		size_t index = token[level];
		// allow only pure ASCII
		if( index >= TRIESZ )
		{
			cout << "Internal error: invalid character in token\n";
			exit(1);
		}
 
		// Create a new child if it doesn't exist already
		if( pCrawl->child[index] == NULL )
			pCrawl->child[index] = new trieNode;
		pCrawl->child[index]->freq++;
 
		// Move to the child
		pCrawl = pCrawl->child[index];
	}
}

// This function returns the length of the unique prefix for the string stored in token
size_t findUniqueLen(trieNode* root, const string& token)
{
	trieNode* pCrawl = root;
	for( size_t i=0; i < token.length(); ++i )
	{
		size_t index = token[i];
		// allow only pure ASCII
		if( index >= TRIESZ )
		{
			cout << "Internal error: invalid character in token\n";
			exit(1);
		}
		pCrawl = pCrawl->child[index];
		if( pCrawl->freq == 1 )
			return i+1;
	}
	// we can get here if the token is a substring of another token, e.g. "NO" and "NORMALIZE"
	return token.length();
}

bool MatchLongParm(const string lopt[], const size_t loml[], size_t nlopt, string& token)
{
	for( size_t i=0; i < nlopt; ++i )
	{
		size_t len = max(loml[i], token.size());
		if( lopt[i].substr(0, len) == token )
		{
			token = lopt[i];
			return true;
		}
	}
	return false;
}

string GetParameter(char** argv, int& i, int j, uint32_t& res)
{
	string num;
	if( j < 0 || int(strlen(argv[i])) == j+1 )
		num = argv[++i];
	else
		num = argv[i]+j+1;
	istringstream iss(num);
	iss >> res;
	if( iss.fail() || !iss.eof() )
		return num;
	else
		return string();
}

int main(int argc, char** argv)
{
	vh_params vhp;
	vhp.cmd = argv[0];
	// no need to check for "32", it is the default
	if( vhp.cmd.find("64") != string::npos )
		vhp.set_hash_width(64);
	else if( vhp.cmd.find("128") != string::npos )
		vhp.set_hash_width(128);
	else if( vhp.cmd.find("256") != string::npos )
		vhp.set_hash_width(256);
	else if( vhp.cmd.find("512") != string::npos )
		vhp.set_hash_width(512);
	else if( vhp.cmd.find("1024") != string::npos )
		vhp.set_hash_width(1024);
	ostringstream oss;
	oss << "VH" << vhp.vh_hash_width;
	vhp.name = oss.str();

	// the alphabetical list of recognized long options 
	static const string lopt[] = {
		"--avx2", "--avx512", "--binary", "--check", "--help", "--ignore-missing", "--length",
		"--quiet", "--scalar", "--sse2", "--status", "--strict", "--tag", "--text", "--verbose",
		"--version", "--warn", "--zero"
	};
	static const size_t nlopt = sizeof(lopt)/sizeof(string);
	size_t loml[nlopt];

	// initialize loml
	trieNode* root = new trieNode;
	for( size_t i=0; i < nlopt; ++i )
		insertToken(root, lopt[i]);
	for( size_t i=0; i < nlopt; ++i )
		loml[i] = findUniqueLen(root, lopt[i]);
	delete root;

	bool lgOptions = true;
	vector<string> fnam;
	for( int i=1; i < argc; ++i )
	{
		string arg = argv[i];
		if( !lgOptions || arg.length() <= 1 || arg[0] != '-' )
		{
			fnam.emplace_back(arg);
		}
		else
		{
			// expand abbreviated long option if necessary
			if( arg.length() > 2 && arg[0] == '-' && arg[1] == '-' )
			{
				if( !MatchLongParm(lopt, loml, nlopt, arg) )
				{
					cout << vhp.cmd << ": unrecognized option \'" << arg << "\'\n";
					cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
					return 1;
				}
			}
			if( isalpha(arg[1]) )
			{
				for( size_t j=1; j < arg.length(); j++ )
				{
					if( arg[j] == 'b' )
					{
						vhp.lgBinarySet = true;
						vhp.lgTextSet = false;
					}
					else if( arg[j] == 'c' )
						vhp.lgCheckMode = true;
					else if( arg[j] == 'h' )
						PrintHelp(vhp);
					else if( arg[j] == 'i' )
						vhp.lgIgnoreMissing = true;
					else if( arg[j] == 'l' )
					{
						uint32_t hw;
						auto s = GetParameter(argv, i, j, hw);
						if( s != string() )
						{
							cout << vhp.cmd << ": invalid length: \'" << s << "\'\n";
							return 1;
						}
						if( !vhp.set_hash_width(hw) )
						{
							cout << vhp.cmd << ": invalid length: \'" << hw << "\'\n";
							cout << vhp.cmd << ": length must be 32, 64, 128, 256, 512, or 1024\n";
							return 1;
						}
						j = arg.length();
					}
					else if( arg[j] == 'q' )
						vhp.lgQuiet = true;
					else if( arg[j] == 'Q' )
						vhp.lgStatusOnly = true;
					else if( arg[j] == 's' )
						vhp.lgStrict = true;
					else if( arg[j] == 't' )
					{
						vhp.lgTextSet = true;
						vhp.lgBinarySet = false;
					}
					else if( arg[j] == 'v' )
						PrintVersion(vhp);	
					else if( arg[j] == 'w' )
						vhp.lgWarnSyntax = true;
					else if( arg[j] == 'z' )
						vhp.lgZero = true;
					else
					{
						cout << vhp.cmd << ": invalid option -- \'" << arg[j] << "\'\n";
						cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
						return 1;
					}
				}
			}
			else if( arg == "--" )
				lgOptions = false;
			else if( arg == "--avx2" )
				vhp.SIMDversion = IS_AVX2;
			else if( arg == "--avx512" )
				vhp.SIMDversion = IS_AVX512;
			else if( arg == "--binary" )
			{
				vhp.lgBinarySet = true;
				vhp.lgTextSet = false;
			}
			else if( arg == "--check" )
				vhp.lgCheckMode = true;
			else if( arg == "--help" )
				PrintHelp(vhp);
			else if( arg == "--ignore-missing" )
				vhp.lgIgnoreMissing = true;
			else if( arg == "--length" )
			{
				uint32_t hw;
				auto s = GetParameter(argv, i, -1, hw);
				if( s != string() )
				{
					cout << vhp.cmd << ": invalid length: \'" << s << "\'\n";
					return 1;
				}
				if( !vhp.set_hash_width(hw) )
				{
					cout << vhp.cmd << ": invalid length: \'" << hw << "\'\n";
					cout << vhp.cmd << ": length must be 32, 64, 128, 256, 512, or 1024\n";
					return 1;
				}
			}
			else if( arg == "--quiet" )
				vhp.lgQuiet = true;
			else if( arg == "--scalar" )
				vhp.SIMDversion = IS_SCALAR;
			else if( arg == "--sse2" )
				vhp.SIMDversion = IS_SSE2;
			else if( arg == "--status" )
				vhp.lgStatusOnly = true;
			else if( arg == "--strict" )
				vhp.lgStrict = true;
			else if( arg == "--tag" )
				vhp.lgBSDstyle = true;
			else if( arg == "--text" )
			{
				vhp.lgTextSet = true;
				vhp.lgBinarySet = false;
			}
			else if( arg == "--verbose" )
				vhp.lgVerbose = true;
			else if( arg == "--version" )
				PrintVersion(vhp);
			else if( arg == "--warn" )
				vhp.lgWarnSyntax = true;
			else if( arg == "--zero" )
				vhp.lgZero = true;
			else
			{
				cout << vhp.cmd << ": unrecognized option \'" << arg << "\'\n";
				cout << "Try \'" << vhp.cmd << " --help\' for more information.\n";
				return 1;
			}
		}
	}

	VerifyOptions( vhp );

	if( fnam.size() == 0 )
	{
		// no file name was given -> process stdin
		ProcessFile( vhp, "-", 0 );
	}
	else
	{
		for( const auto& file : fnam )
		{
			if( file == "-" )
			{
				ProcessFile( vhp, file, 0 );
			}
			else
			{
				FILE* io = fopen( file.c_str(), vhp.option().c_str() );
				if( io == 0 )
				{
					cout << vhp.cmd << ": " << escfn(file) << ": No such file or directory\n";
					vhp.returncode = 1;
				}
				else
				{
					ProcessFile( vhp, file, io );
					fclose( io );
				}
			}
		}
	}

	return vhp.returncode;
}

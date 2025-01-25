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
#include "../cpuid/cpuinfo.hpp"
#include "vectorhash.h"
#include "vectorhash_priv.h"
#include "vectorhash_core.h"
#include "vectorhash_avx512.h"
#include "vectorhash_avx2.h"
#include "vectorhash_sse2.h"
#include "vectorhash_scalar.h"

void stateinit(uint32_t st[], uint32_t& seed, size_t lvh_nint)
{
	for( size_t i=0; i < lvh_nint; i++ )
		st[i] = seed;
	seed = fmix32(seed);
}

is_type GetSIMDVersion()
{
#ifdef VH_INTEL
	// determine which SIMD instruction set should be used to calculate the checksum
	//
	// -1: not initialized yet
	//  0: use scalar version of the algorithm
	//  1: use SSE2 instructions
	//  2: use AVX2 instructions
	//  3: use AVX512F instructions

	cpuid::cpuinfo m_cpuid;
	if( m_cpuid.has_avx512_f() )
		return IS_AVX512;
	else if( m_cpuid.has_avx2() )
		return IS_AVX2;
	else if( m_cpuid.has_sse2() )
		return IS_SSE2;
	else
		return IS_SCALAR;
#else
	return IS_SCALAR;
#endif
}

void VectorHashBody32(const uint32_t* data, uint32_t h1[], uint32_t h2[], uint32_t h3[], uint32_t h4[],
					  size_t hash_width)
{
	if( hash_width == 128 )
		VectorHashBody32_128(data, h1, h2, h3, h4);
	else if( hash_width == 256 )
		VectorHashBody32_256(data, h1, h2, h3, h4);
	else if( hash_width == 512 )
		VectorHashBody32_512(data, h1, h2, h3, h4);
	else if( hash_width == 1024 )
		VectorHashBody32_1024(data, h1, h2, h3, h4);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

void VectorHashBody128(const v4si* data, v4si h1[], v4si h2[], v4si h3[], v4si h4[], size_t hash_width)
{
	if( hash_width == 128 )
		VectorHashBody128_128(data, h1, h2, h3, h4);
	else if( hash_width == 256 )
		VectorHashBody128_256(data, h1, h2, h3, h4);
	else if( hash_width == 512 )
		VectorHashBody128_512(data, h1, h2, h3, h4);
	else if( hash_width == 1024 )
		VectorHashBody128_1024(data, h1, h2, h3, h4);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

void VectorHashBody256(const v8si* data, v8si h1[], v8si h2[], v8si h3[], v8si h4[], size_t hash_width)
{
	if( hash_width == 128 )
		VectorHashBody256_128(data, h1, h2, h3, h4);
	else if( hash_width == 256 )
		VectorHashBody256_256(data, h1, h2, h3, h4);
	else if( hash_width == 512 )
		VectorHashBody256_512(data, h1, h2, h3, h4);
	else if( hash_width == 1024 )
		VectorHashBody256_1024(data, h1, h2, h3, h4);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

void VectorHashBody512(const v16si* data, v16si h1[], v16si h2[], v16si h3[], v16si h4[], size_t hash_width)
{
	if( hash_width == 128 )
		VectorHashBody512_128(data, h1, h2, h3, h4);
	else if( hash_width == 256 )
		VectorHashBody512_256(data, h1, h2, h3, h4);
	else if( hash_width == 512 )
		VectorHashBody512_512(data, h1, h2, h3, h4);
	else if( hash_width == 1024 )
		VectorHashBody512_1024(data, h1, h2, h3, h4);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

static void VectorHash32(const void* buf, size_t len, uint32_t seed, void* out, size_t hash_width)
{
	if( hash_width == 128 )
		VectorHash32_128(buf, len, seed, out);
	else if( hash_width == 256 )
		VectorHash32_256(buf, len, seed, out);
	else if( hash_width == 512 )
		VectorHash32_512(buf, len, seed, out);
	else if( hash_width == 1024 )
		VectorHash32_1024(buf, len, seed, out);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

static void VectorHash128(const void* buf, size_t len, uint32_t seed, void* out, size_t hash_width)
{
	if( hash_width == 128 )
		VectorHash128_128(buf, len, seed, out);
	else if( hash_width == 256 )
		VectorHash128_256(buf, len, seed, out);
	else if( hash_width == 512 )
		VectorHash128_512(buf, len, seed, out);
	else if( hash_width == 1024 )
		VectorHash128_1024(buf, len, seed, out);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

static void VectorHash256(const void* buf, size_t len, uint32_t seed, void* out, size_t hash_width)
{
	if( hash_width == 128 )
		VectorHash256_128(buf, len, seed, out);
	else if( hash_width == 256 )
		VectorHash256_256(buf, len, seed, out);
	else if( hash_width == 512 )
		VectorHash256_512(buf, len, seed, out);
	else if( hash_width == 1024 )
		VectorHash256_1024(buf, len, seed, out);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

static void VectorHash512(const void* buf, size_t len, uint32_t seed, void* out, size_t hash_width)
{
	if( hash_width == 128 )
		VectorHash512_128(buf, len, seed, out);
	else if( hash_width == 256 )
		VectorHash512_256(buf, len, seed, out);
	else if( hash_width == 512 )
		VectorHash512_512(buf, len, seed, out);
	else if( hash_width == 1024 )
		VectorHash512_1024(buf, len, seed, out);
	else {
		cout << "Internal error: impossible value for hash width: " << hash_width << "." << endl;
		exit(1);
	}
}

void VectorHash(const void* buf, size_t len, uint32_t seed, void* out, is_type SIMDversion, size_t hash_width)
{
	auto ibuf = reinterpret_cast<uintptr>(buf);
	// check if the alignment of the pointer in buf is OK
	// AVX512 requires 64-byte alignment
	// AVX2   requires 32-byte alignment
	// SSE2   requires 16-byte alignment
	// Scalar requires no special alignment
	if( SIMDversion >= IS_AVX512 && (ibuf&0x3f) == 0 )
	{
		VectorHash512(buf, len, seed, out, hash_width);
	}
	else if( SIMDversion >= IS_AVX2 && (ibuf&0x1f) == 0 )
	{
		VectorHash256(buf, len, seed, out, hash_width);
	}
	else if( SIMDversion >= IS_SSE2 && (ibuf&0x0f) == 0 )
	{
		VectorHash128(buf, len, seed, out, hash_width);
	}
	else if( SIMDversion >= IS_SCALAR )
	{
		VectorHash32(buf, len, seed, out, hash_width);
	}
	else {
		cout << "Internal error: impossible value for SIMD version: " << SIMDversion << "." << endl;
		exit(1);
	}
}

void VectorHash(const void* buf, size_t len, uint32_t seed, void* out, size_t hash_width)
{
	VectorHash(buf, len, seed, out, GetSIMDVersion(), hash_width);
}

//-------------------------------------------------------------------------------
//  VectorHash - a very fast hash function optimized using SIMD instructions
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#ifndef VECTORHASH_CORE_H
#define VECTORHASH_CORE_H

#include <cstdint>
#include "vectorhash_priv.h"
#include "vectorhash_avx512.h"
#include "vectorhash_avx2.h"
#include "vectorhash_sse2.h"
#include "vectorhash_scalar.h"

void stateinit(uint32_t st[], uint32_t& seed, size_t lvh_nint);
is_type GetSIMDVersion();
void VectorHashBody32(const uint32_t* data, uint32_t h1[], uint32_t h2[], uint32_t h3[], uint32_t h4[],
					  size_t hash_width);
void VectorHashBody128(const v4si* data, v4si h1[], v4si h2[], v4si h3[], v4si h4[], size_t hash_width);
void VectorHashBody256(const v8si* data, v8si h1[], v8si h2[], v8si h3[], v8si h4[], size_t hash_width);
void VectorHashBody512(const v16si* data, v16si h1[], v16si h2[], v16si h3[], v16si h4[], size_t hash_width);
void VectorHash(const void* buf, size_t len, uint32_t seed, void* out, is_type SIMDversion, size_t hash_width);

inline uint32_t fmix32 ( uint32_t h0, uint32_t h1 = 0xd86b048b )
{
	h1 ^= h0;
	h0 = ROTL32(h0, 11) ^ h1 ^ ROTL32(h1, 13);
	h1 = ROTL32(h1, 19);
	return h0 + h1;
}

#endif

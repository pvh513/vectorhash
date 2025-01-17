//-------------------------------------------------------------------------------
//  Vectorhash - a very fast vectorized hash function optimized for large buffers
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#ifndef VECTORHASH_SSE2_H
#define VECTORHASH_SSE2_H

#include <immintrin.h>
#include "vectorhash_priv.h"

typedef __m128i v4si;

void VectorHashBody128_128(const v4si* data, v4si h1[], v4si h2[], v4si h3[], v4si h4[]);
void VectorHashBody128_256(const v4si* data, v4si h1[], v4si h2[], v4si h3[], v4si h4[]);
void VectorHashBody128_512(const v4si* data, v4si h1[], v4si h2[], v4si h3[], v4si h4[]);
void VectorHashBody128_1024(const v4si* data, v4si h1[], v4si h2[], v4si h3[], v4si h4[]);

void VectorHash128_128(const void* key, size_t len, uint32_t seed, void* out);
void VectorHash128_256(const void* key, size_t len, uint32_t seed, void* out);
void VectorHash128_512(const void* key, size_t len, uint32_t seed, void* out);
void VectorHash128_1024(const void* key, size_t len, uint32_t seed, void* out);

#endif

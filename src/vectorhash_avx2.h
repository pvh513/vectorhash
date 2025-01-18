//-------------------------------------------------------------------------------
//  VectorHash - a very fast hash function optimized using SIMD instructions
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#ifndef VECTORHASH_AVX2_H
#define VECTORHASH_AVX2_H

#include <immintrin.h>
#include "vectorhash_priv.h"

typedef __m256i v8si;

void VectorHashBody256_128(const v8si* data, v8si h1[], v8si h2[], v8si h3[], v8si h4[]);
void VectorHashBody256_256(const v8si* data, v8si h1[], v8si h2[], v8si h3[], v8si h4[]);
void VectorHashBody256_512(const v8si* data, v8si h1[], v8si h2[], v8si h3[], v8si h4[]);
void VectorHashBody256_1024(const v8si* data, v8si h1[], v8si h2[], v8si h3[], v8si h4[]);

void VectorHash256_128(const void* key, size_t len, uint32_t seed, void* out);
void VectorHash256_256(const void* key, size_t len, uint32_t seed, void* out);
void VectorHash256_512(const void* key, size_t len, uint32_t seed, void* out);
void VectorHash256_1024(const void* key, size_t len, uint32_t seed, void* out);

#endif

//-------------------------------------------------------------------------------
//  VectorHash - a very fast hash function optimized using SIMD instructions
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#ifndef VECTORHASH_H
#define VECTORHASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void VectorHash(const void* buf, size_t len, uint32_t seed, void* out, size_t hash_width);

#ifdef __cplusplus
}
#endif

#endif

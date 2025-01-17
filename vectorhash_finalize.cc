//-------------------------------------------------------------------------------
//  Vectorhash - a very fast vectorized hash function optimized for large buffers
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#include "vectorhash_priv.h"
#include "vectorhash_core.h"

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

void EXT(VectorHashFinalize)(size_t len, uint32_t* h1, uint32_t* h2, uint32_t* h3, uint32_t* h4, void* out)
{
	uint32_t lenc = fmix32(len & 0x7fffffff);

	VEC( vh_nint, h1[j] ^= lenc );
	VEC( vh_nint, h2[j] ^= lenc );
	VEC( vh_nint, h3[j] ^= lenc );
	VEC( vh_nint, h4[j] ^= lenc );

	for( size_t j=1; j < vh_nint; j++ )
	{
		h1[0] += h2[j];
		h2[0] += h3[j];
		h3[0] += h4[j];
		h4[0] += h1[j];
		h1[j] += h2[0];
		h2[j] += h3[0];
		h3[j] += h4[0];
		h4[j] += h1[0];
	}

	VEC( vh_nint, h1[j] += h2[j] );
	VEC( vh_nint, h1[j] += h3[j] );
	VEC( vh_nint, h1[j] += h4[j] );
	VEC( vh_nint, h2[j] += h1[j] );
	VEC( vh_nint, h3[j] += h1[j] );
	VEC( vh_nint, h4[j] += h1[j] );
	VEC( vh_nint, h1[j] = fmix32(h1[j], h3[j]) );
	VEC( vh_nint, h2[j] = fmix32(h2[j], h4[j]) );
	VEC( vh_nint, h3[j] = fmix32(h3[j], h1[j]) );
	VEC( vh_nint, h4[j] = fmix32(h4[j], h2[j]) );
	VEC( vh_nint, h1[j] += h2[j] );
	VEC( vh_nint, h1[j] += h3[j] );
	VEC( vh_nint, h1[j] += h4[j] );
	VEC( vh_nint, h2[j] += h1[j] );
	VEC( vh_nint, h3[j] += h1[j] );
	VEC( vh_nint, h4[j] += h1[j] );

	for( size_t j=1; j < vh_nint; j++ )
	{
		h1[0] += h4[j];
		h2[0] += h1[j];
		h3[0] += h2[j];
		h4[0] += h3[j];
		h1[j] += h4[0];
		h2[j] += h1[0];
		h3[j] += h2[0];
		h4[j] += h3[0];
	}

	uint32_t* res = (uint32_t*)out;
	uint32_t* z[vh_nstate];
	size_t j = 0;
	size_t nn = vh_nint*4/vh_nstate;
	for( size_t i=0; i < vh_nstate/4; i++ )
		z[j++] = &h1[i*nn];
	for( size_t i=0; i < vh_nstate/4; i++ )
		z[j++] = &h2[i*nn];
	for( size_t i=0; i < vh_nstate/4; i++ )
		z[j++] = &h3[i*nn];
	for( size_t i=0; i < vh_nstate/4; i++ )
		z[j++] = &h4[i*nn];

	// fold the result to the desired hash width
	uint32_t narr = vh_hash_width/32;
	uint32_t nelem = 4*vh_virtreg_width/vh_hash_width;
	for( uint32_t i=0; i < narr; i++ )
	{
		res[i] = z[i][0];
		for( size_t j=1; j < nelem; j++ )
			res[i] ^= z[(j+i)%narr][j];
	}
}


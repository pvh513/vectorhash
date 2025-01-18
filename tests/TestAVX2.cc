//-------------------------------------------------------------------------------
//  VectorHash - a very fast hash function optimized using SIMD instructions
//
//  Copyright (c) 2018-2025 Peter A.M. van Hoof
//  All Rights Reserved
//
//  Distributed under the "zlib license". See the accompanying LICENSE file.
//-------------------------------------------------------------------------------

#include "TestMain.h"
#include "../vectorhash_avx2.h"

namespace {

	// test empty buffer
	TEST(TestZeroLengthBufferAVX2_128)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			VectorHash256_128(buffer, 0, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "30a574ab9824cb4358b310345eb60ab0") );
		}
	}

	TEST(TestZeroLengthBufferAVX2_256)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			VectorHash256_256(buffer, 0, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "bb92155ef2049290deea8ca97f4d88d39c0f2490b49e30c483c26086ede6520a") );
		}
	}

	TEST(TestZeroLengthBufferAVX2_512)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			VectorHash256_512(buffer, 0, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "0bf187c2657c96361df020771e2ef920513ad53d86fae25a99b1f9f5c76b7aa7"
			                        "2281c0e4c16318bd92fb5627c61677a09da0f950210f5f9f4a4ebf7e9a5ff0dc") );
		}
	}

	TEST(TestZeroLengthBufferAVX2_1024)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			VectorHash256_1024(buffer, 0, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "65ab6b7d081119602b80cdd8296d732f682986d56277d990bb4a002f5ad2eca0"
			                        "dd963bf652c3a82dea739e42774b49a035760ccbddae9c4641757d7bd7e04f48"
			                        "a72b89039e1a2dbebb140f3e83813d85f637d5195c3f9c5f023510df0b94f81d"
			                        "275bc94224ceb3b9256ea207318abddbba5e04c9fcd8e8725253652d1c55f3ae") );
		}
	}

	// test half a blocksize
	TEST(Test128ByteBufferAVX2_128)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test128", 128, buffer) );
			VectorHash256_128(buffer, 128, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "1fdd011c819e318cbb24f4e7dd2e5421") );
		}
	}

	TEST(Test128ByteBufferAVX2_256)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test128", 128, buffer) );
			VectorHash256_256(buffer, 128, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "d219785d3819b42292b1195cbcd407addb2bdd52fbf8dc43f7521c98ebad5f51") );
		}
	}

	TEST(Test256ByteBufferAVX2_512)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test256", 256, buffer) );
			VectorHash256_512(buffer, 256, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "df76c929147743beb82c14d8aa9168ce05b3648460da5770ce720a88737aa2e3"
		                            "9b8fcef343ab8993ab981878ea0b94ed9be2014b8099399cd41ffa4972cd05f7") );
		}
	}

	TEST(Test512ByteBufferAVX2_1024)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test512", 512, buffer) );
			VectorHash256_1024(buffer, 512, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "625d7810e2315de7875b0c47a5657d4caf5a672b8b3d155856b5b7e065b55c25"
		                            "0a95071b8159c0d7a4bba19c6fac3c089136166f01bcffee2021e95be5c30766"
		                            "534ffa4313a07314da701c6e2255bfb0dd90f0005ae8f73327696a95638aeaa0"
		                            "e9331eef20ca8016499c4ac548b00d4a7da04afde66519ab8ae5461d500f0006") );
		}
	}

	// test one blocksize
	TEST(Test256ByteBufferAVX2_128)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test256", 256, buffer) );
			VectorHash256_128(buffer, 256, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "f6abb56afb29301b5d9ea79fa7c239e4") );
		}
	}

	TEST(Test256ByteBufferAVX2_256)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test256", 256, buffer) );
			VectorHash256_256(buffer, 256, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "fa0040151e96185570d156c1b5727ec5abfe80e90ae45544badcf930cd2d47d3") );
		}
	}

	TEST(Test512ByteBufferAVX2_512)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test512", 512, buffer) );
			VectorHash256_512(buffer, 512, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "ea1391390c88e41067dbd0f877c9af1a95295eb64c6f3f9b8a920613cc63b446"
		                            "b36d5bae4bc9f4c0a8a0376848269c8957cf38579c75f207821d91b45025f123") );
		}
	}

	TEST(Test1024ByteBufferAVX2_1024)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1024", 1024, buffer) );
			VectorHash256_1024(buffer, 1024, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "5627edb8658b286cc023cf5295490129f86caf3fef22c569351b22c315e2410c"
		                            "79e6c324e4e62b95ccdbc60d2b3edec2009898d27cd964a4fa3a3429ebb42b3e"
		                            "76033b927653b916fc8a96c9ea0f51cad0a87a79bc9632aa31f3975b9c310fe8"
		                            "b59d388420b5ed788dcd0cfcbb585f022c692253c4f6b618efa47923853a8f9f") );
		}
	}

	// test 3/2 blocksizes
	TEST(Test384ByteBufferAVX2_128)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test384", 384, buffer) );
			VectorHash256_128(buffer, 384, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "9d7bb6d9506f3bd52d39ab74eef95c88") );
		}
	}

	TEST(Test384ByteBufferAVX2_256)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test384", 384, buffer) );
			VectorHash256_256(buffer, 384, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "577bbf1d4f48a54c9690257807fcaa25f2fdf4066ffc824a88e8162592628f95") );
		}
	}

	TEST(Test768ByteBufferAVX2_512)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test768", 768, buffer) );
			VectorHash256_512(buffer, 768, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "5a60e8b008cec8ee86ef487b09309ae320bb2ef01cfd605bd06c2d50b31b1f62"
		                            "bea5f235de35a1863895933a52483a1d6568e4b2fc95ebb619059d9e6cd3e32c") );
		}
	}

	TEST(Test1536ByteBufferAVX2_1024)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1536", 1536, buffer) );
			VectorHash256_1024(buffer, 1536, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "a96be49fa0e0aa28d01c0e629a191fd8bbe193dbdcf513623544f1e27fec4434"
		                            "b598fc6ec746157f55a8ea38ed883dbd96f5523a44e8d873f48b15143ac4978e"
		                            "8b98d9357b8184dc0e6dd1b6e95042d524ec0837763af3c11a751943fe1c117f"
		                            "a5ffac5121aaa2e617db2665d097d3df5295c8b912faef64ca3e1ca65f9c6611") );
		}
	}

	// test a large buffer
	TEST(TestLargeBufferAVX2_128)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_128(buffer, 1048576, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "dad2070b6fbec87754130ef0a5f22b4a") );
		}
	}

	TEST(TestLargeBufferAVX2_256)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_256(buffer, 1048576, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "8c6ae4f3dcaa62bb19c7d05ace46dcf69bd5048e8179000fcdc9e50514a981a6") );
		}
	}

	TEST(TestLargeBufferAVX2_512)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_512(buffer, 1048576, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "677069f5cf26c1a626dfef47d35e697df34c8f3cb592b17681307a9aec2af589"
		                            "91d7fe9dc768cdeee144105627eb1ed687bd26eeb14b43025d754bf2fb3c8cb5") );
		}
	}

	TEST(TestLargeBufferAVX2_1024)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_1024(buffer, 1048576, 0xfd4c799d, cksum);
			CHECK( CheckHash(cksum, "181ea4a710cb01cd12f038f3b4df6760fc1fda8c3424f528ae8e82b0d4762980"
		                            "026c0314630da65b5b3a2dc002fe8d65276a0ee974cee5fa422440a324d7b937"
		                            "72dfa9625cf6c023ca0c4c102f4a148add77413c9637d03603565da7ff98a626"
		                            "22f2934cbae7e6139821a8cf2771f6d6a8228b6fe04df09544f485ac5d4c3531") );
		}
	}

	// test alternative seed
	TEST(TestLargeBufferAltSeedAVX2_128)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_128(buffer, 1048576, 0x6ec74615, cksum);
			CHECK( CheckHash(cksum, "27a01fc4e77102052e8a2365a34e0e13") );
		}
	}

	TEST(TestLargeBufferAltSeedAVX2_256)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_256(buffer, 1048576, 0x6ec74615, cksum);
			CHECK( CheckHash(cksum, "b0e45309215ec0f2b081bceb972dc7c2893c5ddeb78d0d0be2b1c7c327034f73") );
		}
	}

	TEST(TestLargeBufferAltSeedAVX2_512)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_512(buffer, 1048576, 0x6ec74615, cksum);
			CHECK( CheckHash(cksum, "67559c4b5fae5379e58577aeb97252436f31a6dc7d66077d978367da6ac1a34f"
		                            "ffb8cef8db00f900003c90a2ce718686bade1a13e35d80ab5226cbbcf35f6035") );
		}
	}

	TEST(TestLargeBufferAltSeedAVX2_1024)
	{
		if( SIMDversion >= IS_AVX2 )
		{
			CHECK( ReadBuffer("test1M", 1048576, buffer) );
			VectorHash256_1024(buffer, 1048576, 0x6ec74615, cksum);
			CHECK( CheckHash(cksum, "ec67116d32b6d835a99c41cac254a561b9f20e792cfdf33e6905d03fee7686e1"
		                            "a60c6ac5f5bdf359d318043b3a15a8723c606fbccf57725697c37184916d43f1"
		                            "ffbc74ba744ab302ce9b38947e87a5d8923759058191fd2bb4a2f301d8834dfe"
		                            "6d0a4ed08680ab3106a5b06ca13d915680079f4f6eefa1376947a64660285e33") );
		}
	}

}

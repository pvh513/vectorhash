## VectorHash Changelog

VectorHash is a very fast, non-cryptographic checksum algorithm. It has in part
been inspired by the PRNG algorithm xoroshiro128++ written by David Blackman and
Sebastiano Vigna (for the core of the hashing function) and MurmurHash3 written
by Austin Appleby (for the finalization mix and some support routines). This
program has been written such that it can be vectorized using SIMD instructions
from the SSE2, AVX2, or AVX512f instruction sets, which makes it very fast. It
also has a scalar version for non-Intel platforms. The resulting checksum is
identical for each of these instruction sets. The supported instruction sets
will be detected automatically during runtime.

### Release Strategy

The initial development will be aimed at finalizing the main functionality of the code
and doing more advanced tests in order to flush out the remaining bugs. During
this time beta releases will be created with version numbers v0.x. The aim is,
amongst other things, to run a clean test suite for all checksum widths against
SMHasher written by ZhuReini Urban (except for the small key speed test). Since
testing against SMHasher is not fully complete, it may happen that checksums
change during the beta tesing phase. Once beta testing is complete, version
number v1.0 will be created as the first official release. From that point
onwards backward compatibility will be guaranteed and the checksums will stay
the same (at least for versions v1.x).

### Version History

#### Release v0.1 was created on 3 Feb 2025.

- Fix bugs that resulted in incorrect checksums when reading from stdin.
  Checksums created when reading from a named file remain unchanged.

- Add numerous tests of the command-line executable.

#### Release v0.2 was created on 9 Feb 2025.

- Fix numerous minor bugs to improve compatibility with standard Linux
  checksum algorithms.

#### Release v0.3 was created on 14 Feb 2025.

- Add support for building 32-bit libraries on 64-bit operating systems.

#### Release v0.4 was created on 16 Feb 2025.

- Add support for additional checksum widths. Now all widths that are
  a multiple of 32 between 32 and 1024 are supported. Not all widths
  will be tested against SMHasher, but this will be done for all widths
  up to 256, as well as 512 and 1024.

#### Release v0.5 was created on 26 Mar 2025.

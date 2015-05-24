/* ====================================================================
 * Copyright (c) 2003 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include <openssl/err.h>
#include <openssl/fips.h>
#include <openssl/rand.h>
#include <openssl/fips_rand.h>

#ifdef OPENSSL_FIPS

typedef struct {
    unsigned char DT[16];
    unsigned char V[16];
    unsigned char R[16];
} AES_PRNG_TV;

/* The following test vectors are taken directly from the RGNVS spec */

static unsigned char aes_128_key[16] =
    { 0xf3, 0xb1, 0x66, 0x6d, 0x13, 0x60, 0x72, 0x42,
    0xed, 0x06, 0x1c, 0xab, 0xb8, 0xd4, 0x62, 0x02
};

static AES_PRNG_TV aes_128_tv[] = {
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x22, 0xf9},
     /* V */
     {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x59, 0x53, 0x1e, 0xd1, 0x3b, 0xb0, 0xc0, 0x55,
      0x84, 0x79, 0x66, 0x85, 0xc1, 0x2f, 0x76, 0x41}
     },
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x22, 0xfa},
     /* V */
     {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x7c, 0x22, 0x2c, 0xf4, 0xca, 0x8f, 0xa2, 0x4c,
      0x1c, 0x9c, 0xb6, 0x41, 0xa9, 0xf3, 0x22, 0x0d}
     },
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x22, 0xfb},
     /* V */
     {0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x8a, 0xaa, 0x00, 0x39, 0x66, 0x67, 0x5b, 0xe5,
      0x29, 0x14, 0x28, 0x81, 0xa9, 0x4d, 0x4e, 0xc7}
     },
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x22, 0xfc},
     /* V */
     {0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x88, 0xdd, 0xa4, 0x56, 0x30, 0x24, 0x23, 0xe5,
      0xf6, 0x9d, 0xa5, 0x7e, 0x7b, 0x95, 0xc7, 0x3a}
     },
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x22, 0xfd},
     /* V */
     {0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x05, 0x25, 0x92, 0x46, 0x61, 0x79, 0xd2, 0xcb,
      0x78, 0xc4, 0x0b, 0x14, 0x0a, 0x5a, 0x9a, 0xc8}
     },
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x23, 0x77},
     /* V */
     {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe},
     /* R */
     {0x0d, 0xd5, 0xa0, 0x36, 0x7a, 0x59, 0x26, 0xbc,
      0x48, 0xd9, 0x38, 0xbf, 0xf0, 0x85, 0x8f, 0xea}
     },
    {
     /* DT */
     {0xe6, 0xb3, 0xbe, 0x78, 0x2a, 0x23, 0xfa, 0x62,
      0xd7, 0x1d, 0x4a, 0xfb, 0xb0, 0xe9, 0x23, 0x78},
     /* V */
     {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
     /* R */
     {0xae, 0x53, 0x87, 0xee, 0x8c, 0xd9, 0x12, 0xf5,
      0x73, 0x53, 0xae, 0x03, 0xf9, 0xd5, 0x13, 0x33}
     },
};

static unsigned char aes_192_key[24] =
    { 0x15, 0xd8, 0x78, 0x0d, 0x62, 0xd3, 0x25, 0x6e,
    0x44, 0x64, 0x10, 0x13, 0x60, 0x2b, 0xa9, 0xbc,
    0x4a, 0xfb, 0xca, 0xeb, 0x4c, 0x8b, 0x99, 0x3b
};

static AES_PRNG_TV aes_192_tv[] = {
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0x4b},
     /* V */
     {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x17, 0x07, 0xd5, 0x28, 0x19, 0x79, 0x1e, 0xef,
      0xa5, 0x0c, 0xbf, 0x25, 0xe5, 0x56, 0xb4, 0x93}
     },
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0x4c},
     /* V */
     {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x92, 0x8d, 0xbe, 0x07, 0xdd, 0xc7, 0x58, 0xc0,
      0x6f, 0x35, 0x41, 0x9b, 0x17, 0xc9, 0xbd, 0x9b}
     },
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0x4d},
     /* V */
     {0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0xd5, 0xde, 0xf4, 0x50, 0xf3, 0xb7, 0x10, 0x4e,
      0xb8, 0xc6, 0xf8, 0xcf, 0xe2, 0xb1, 0xca, 0xa2}
     },
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0x4e},
     /* V */
     {0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0xce, 0x29, 0x08, 0x43, 0xfc, 0x34, 0x41, 0xe7,
      0x47, 0x8f, 0xb3, 0x66, 0x2b, 0x46, 0xb1, 0xbb}
     },
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0x4f},
     /* V */
     {0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0xb3, 0x26, 0x0f, 0xf5, 0xd6, 0xca, 0xa8, 0xbf,
      0x89, 0xb8, 0x5e, 0x2f, 0x22, 0x56, 0x92, 0x2f}
     },
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0xc9},
     /* V */
     {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe},
     /* R */
     {0x05, 0xeb, 0x18, 0x52, 0x34, 0x43, 0x00, 0x43,
      0x6e, 0x5a, 0xa5, 0xfe, 0x7b, 0x32, 0xc4, 0x2d}
     },
    {
     /* DT */
     {0x3f, 0xd8, 0xff, 0xe8, 0x80, 0x69, 0x8b, 0xc1,
      0xbf, 0x99, 0x7d, 0xa4, 0x24, 0x78, 0xf3, 0xca},
     /* V */
     {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
     /* R */
     {0x15, 0x3c, 0xe8, 0xd1, 0x04, 0xc7, 0xad, 0x50,
      0x0b, 0xf0, 0x07, 0x16, 0xe7, 0x56, 0x7a, 0xea}
     },
};

static unsigned char aes_256_key[32] =
    { 0x6d, 0x14, 0x06, 0x6c, 0xb6, 0xd8, 0x21, 0x2d,
    0x82, 0x8d, 0xfa, 0xf2, 0x7a, 0x03, 0xb7, 0x9f,
    0x0c, 0xc7, 0x3e, 0xcd, 0x76, 0xeb, 0xee, 0xb5,
    0x21, 0x05, 0x8c, 0x4f, 0x31, 0x7a, 0x80, 0xbb
};

static AES_PRNG_TV aes_256_tv[] = {
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9e, 0x88},
     /* V */
     {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x35, 0xc7, 0xef, 0xa7, 0x78, 0x4d, 0x29, 0xbc,
      0x82, 0x79, 0x99, 0xfb, 0xd0, 0xb3, 0x3b, 0x72}
     },
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9e, 0x89},
     /* V */
     {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x6c, 0xf4, 0x42, 0x5d, 0xc7, 0x04, 0x1a, 0x41,
      0x28, 0x2a, 0x78, 0xa9, 0xb0, 0x12, 0xc4, 0x95}
     },
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9e, 0x8a},
     /* V */
     {0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x16, 0x90, 0xa4, 0xff, 0x7b, 0x7e, 0xb9, 0x30,
      0xdb, 0x67, 0x4b, 0xac, 0x2d, 0xe1, 0xd1, 0x75}
     },
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9e, 0x8b},
     /* V */
     {0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x14, 0x6f, 0xf5, 0x95, 0xa1, 0x46, 0x65, 0x30,
      0xbc, 0x57, 0xe2, 0x4a, 0xf7, 0x45, 0x62, 0x05}
     },
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9e, 0x8c},
     /* V */
     {0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
     /* R */
     {0x96, 0xe2, 0xb4, 0x1e, 0x66, 0x5e, 0x0f, 0xa4,
      0xc5, 0xcd, 0xa2, 0x07, 0xcc, 0xb7, 0x94, 0x40}
     },
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9f, 0x06},
     /* V */
     {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe},
     /* R */
     {0x61, 0xce, 0x1d, 0x6a, 0x48, 0x75, 0x97, 0x28,
      0x4b, 0x41, 0xde, 0x18, 0x44, 0x4f, 0x56, 0xec}
     },
    {
     /* DT */
     {0xda, 0x3a, 0x41, 0xec, 0x1d, 0xa3, 0xb0, 0xd5,
      0xf2, 0xa9, 0x4e, 0x34, 0x74, 0x8e, 0x9f, 0x07},
     /* V */
     {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
     /* R */
     {0x52, 0x89, 0x59, 0x79, 0x2d, 0xaa, 0x28, 0xb3,
      0xb0, 0x8a, 0x3e, 0x70, 0xfa, 0x71, 0x59, 0x84}
     },
};

void FIPS_corrupt_rng()
{
    aes_192_tv[0].V[0]++;
}

# define fips_rand_test(key, tv) \
        do_rand_test(key, sizeof key, tv, sizeof(tv)/sizeof(AES_PRNG_TV))

static int do_rand_test(unsigned char *key, int keylen,
                        AES_PRNG_TV * tv, int ntv)
{
    unsigned char R[16];
    int i;
    if (!FIPS_rand_set_key(key, keylen))
        return 0;
    for (i = 0; i < ntv; i++) {
        FIPS_rand_seed(tv[i].V, 16);
        FIPS_rand_set_dt(tv[i].DT);
        FIPS_rand_bytes(R, 16);
        if (memcmp(R, tv[i].R, 16))
            return 0;
    }
    return 1;
}

int FIPS_selftest_rng()
{
    FIPS_rand_reset();
    if (!FIPS_rand_test_mode()) {
        FIPSerr(FIPS_F_FIPS_SELFTEST_RNG, FIPS_R_SELFTEST_FAILED);
        return 0;
    }
    if (!fips_rand_test(aes_128_key, aes_128_tv)
        || !fips_rand_test(aes_192_key, aes_192_tv)
        || !fips_rand_test(aes_256_key, aes_256_tv)) {
        FIPSerr(FIPS_F_FIPS_SELFTEST_RNG, FIPS_R_SELFTEST_FAILED);
        return 0;
    }
    FIPS_rand_reset();
    return 1;
}

#endif

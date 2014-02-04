#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

static const uint32_t rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000,
};

static const uint8_t Te[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};

static const uint8_t Td[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

static const uint32_t TE[256] = {
    0xc66363a5, 0xf87c7c84, 0xee777799, 0xf67b7b8d, 0xfff2f20d, 0xd66b6bbd, 0xde6f6fb1, 0x91c5c554,
    0x60303050, 0x02010103, 0xce6767a9, 0x562b2b7d, 0xe7fefe19, 0xb5d7d762, 0x4dababe6, 0xec76769a,
    0x8fcaca45, 0x1f82829d, 0x89c9c940, 0xfa7d7d87, 0xeffafa15, 0xb25959eb, 0x8e4747c9, 0xfbf0f00b,
    0x41adadec, 0xb3d4d467, 0x5fa2a2fd, 0x45afafea, 0x239c9cbf, 0x53a4a4f7, 0xe4727296, 0x9bc0c05b,
    0x75b7b7c2, 0xe1fdfd1c, 0x3d9393ae, 0x4c26266a, 0x6c36365a, 0x7e3f3f41, 0xf5f7f702, 0x83cccc4f,
    0x6834345c, 0x51a5a5f4, 0xd1e5e534, 0xf9f1f108, 0xe2717193, 0xabd8d873, 0x62313153, 0x2a15153f,
    0x0804040c, 0x95c7c752, 0x46232365, 0x9dc3c35e, 0x30181828, 0x379696a1, 0x0a05050f, 0x2f9a9ab5,
    0x0e070709, 0x24121236, 0x1b80809b, 0xdfe2e23d, 0xcdebeb26, 0x4e272769, 0x7fb2b2cd, 0xea75759f,
    0x1209091b, 0x1d83839e, 0x582c2c74, 0x341a1a2e, 0x361b1b2d, 0xdc6e6eb2, 0xb45a5aee, 0x5ba0a0fb,
    0xa45252f6, 0x763b3b4d, 0xb7d6d661, 0x7db3b3ce, 0x5229297b, 0xdde3e33e, 0x5e2f2f71, 0x13848497,
    0xa65353f5, 0xb9d1d168, 0x00000000, 0xc1eded2c, 0x40202060, 0xe3fcfc1f, 0x79b1b1c8, 0xb65b5bed,
    0xd46a6abe, 0x8dcbcb46, 0x67bebed9, 0x7239394b, 0x944a4ade, 0x984c4cd4, 0xb05858e8, 0x85cfcf4a,
    0xbbd0d06b, 0xc5efef2a, 0x4faaaae5, 0xedfbfb16, 0x864343c5, 0x9a4d4dd7, 0x66333355, 0x11858594,
    0x8a4545cf, 0xe9f9f910, 0x04020206, 0xfe7f7f81, 0xa05050f0, 0x783c3c44, 0x259f9fba, 0x4ba8a8e3,
    0xa25151f3, 0x5da3a3fe, 0x804040c0, 0x058f8f8a, 0x3f9292ad, 0x219d9dbc, 0x70383848, 0xf1f5f504,
    0x63bcbcdf, 0x77b6b6c1, 0xafdada75, 0x42212163, 0x20101030, 0xe5ffff1a, 0xfdf3f30e, 0xbfd2d26d,
    0x81cdcd4c, 0x180c0c14, 0x26131335, 0xc3ecec2f, 0xbe5f5fe1, 0x359797a2, 0x884444cc, 0x2e171739,
    0x93c4c457, 0x55a7a7f2, 0xfc7e7e82, 0x7a3d3d47, 0xc86464ac, 0xba5d5de7, 0x3219192b, 0xe6737395,
    0xc06060a0, 0x19818198, 0x9e4f4fd1, 0xa3dcdc7f, 0x44222266, 0x542a2a7e, 0x3b9090ab, 0x0b888883,
    0x8c4646ca, 0xc7eeee29, 0x6bb8b8d3, 0x2814143c, 0xa7dede79, 0xbc5e5ee2, 0x160b0b1d, 0xaddbdb76,
    0xdbe0e03b, 0x64323256, 0x743a3a4e, 0x140a0a1e, 0x924949db, 0x0c06060a, 0x4824246c, 0xb85c5ce4,
    0x9fc2c25d, 0xbdd3d36e, 0x43acacef, 0xc46262a6, 0x399191a8, 0x319595a4, 0xd3e4e437, 0xf279798b,
    0xd5e7e732, 0x8bc8c843, 0x6e373759, 0xda6d6db7, 0x018d8d8c, 0xb1d5d564, 0x9c4e4ed2, 0x49a9a9e0,
    0xd86c6cb4, 0xac5656fa, 0xf3f4f407, 0xcfeaea25, 0xca6565af, 0xf47a7a8e, 0x47aeaee9, 0x10080818,
    0x6fbabad5, 0xf0787888, 0x4a25256f, 0x5c2e2e72, 0x381c1c24, 0x57a6a6f1, 0x73b4b4c7, 0x97c6c651,
    0xcbe8e823, 0xa1dddd7c, 0xe874749c, 0x3e1f1f21, 0x964b4bdd, 0x61bdbddc, 0x0d8b8b86, 0x0f8a8a85,
    0xe0707090, 0x7c3e3e42, 0x71b5b5c4, 0xcc6666aa, 0x904848d8, 0x06030305, 0xf7f6f601, 0x1c0e0e12,
    0xc26161a3, 0x6a35355f, 0xae5757f9, 0x69b9b9d0, 0x17868691, 0x99c1c158, 0x3a1d1d27, 0x279e9eb9,
    0xd9e1e138, 0xebf8f813, 0x2b9898b3, 0x22111133, 0xd26969bb, 0xa9d9d970, 0x078e8e89, 0x339494a7,
    0x2d9b9bb6, 0x3c1e1e22, 0x15878792, 0xc9e9e920, 0x87cece49, 0xaa5555ff, 0x50282878, 0xa5dfdf7a,
    0x038c8c8f, 0x59a1a1f8, 0x09898980, 0x1a0d0d17, 0x65bfbfda, 0xd7e6e631, 0x844242c6, 0xd06868b8,
    0x824141c3, 0x299999b0, 0x5a2d2d77, 0x1e0f0f11, 0x7bb0b0cb, 0xa85454fc, 0x6dbbbbd6, 0x2c16163a,
};

static uint8_t byte(uint32_t x, int n)
{
    return (uint8_t)(x >> (8 * n));
}

static uint32_t rotr32(uint32_t x, int n)
{
    return _rotr(x, n);
}

static uint32_t setup_mix(uint32_t temp)
{
    return (Te[byte(temp, 2)] << 24)
         ^ (Te[byte(temp, 1)] << 16)
         ^ (Te[byte(temp, 0)] << 8)
         ^  Te[byte(temp, 3)];
}

static uint32_t setup_mix2(uint32_t temp)
{
    return rotr32(TE[Td[byte(temp, 3)]], 0)
         ^ rotr32(TE[Td[byte(temp, 2)]], 8)
         ^ rotr32(TE[Td[byte(temp, 1)]], 16)
         ^ rotr32(TE[Td[byte(temp, 0)]], 24);
}

template <bool reversed>
uint32_t load(uint32_t x);

template <>
uint32_t load<true>(uint32_t x)
{
    return x;
}

template <>
uint32_t load<false>(uint32_t x)
{
#ifdef _MSC_VER
    return _byteswap_ulong(x);
#else
    return __builtin_bswap32(x);
#endif
}

template <bool reversed>
void store(uint32_t x, uint8_t* ptr);

template <>
void store<true>(uint32_t x, uint8_t* ptr)
{
    (uint32_t&)*ptr = x;
}

template <>
void store<false>(uint32_t x, uint8_t* ptr)
{
#ifdef _MSC_VER
    (uint32_t&)*ptr = _byteswap_ulong(x);
#else
    (uint32_t&)*ptr = __builtin_bswap32(x);
#endif
}

template <bool reversed>
static bool aes128_detect_enc(const uint32_t* ctx, uint8_t* key)
{
    const uint32_t* ptr = ctx;

    uint32_t tmp[8];
    tmp[0] = load<reversed>(ctx[0]);
    tmp[1] = load<reversed>(ctx[1]);
    tmp[2] = load<reversed>(ctx[2]);
    tmp[3] = load<reversed>(ctx[3]);

    for (int i = 0; ctx += 4, i < 10; i++)
    {
        tmp[4] = tmp[0] ^ setup_mix(tmp[3]) ^ rcon[i];
        if (tmp[4] != load<reversed>(ctx[0])) return false;

        tmp[5] = tmp[1] ^ tmp[4];
        if (tmp[5] != load<reversed>(ctx[1])) return false;

        tmp[6] = tmp[2] ^ tmp[5];
        if (tmp[6] != load<reversed>(ctx[2])) return false;

        tmp[7] = tmp[3] ^ tmp[6];
        if (tmp[7] != load<reversed>(ctx[3])) return false;

        tmp[0] = tmp[4];
        tmp[1] = tmp[5];
        tmp[2] = tmp[6];
        tmp[3] = tmp[7];
    }

    store<false>(load<reversed>(ptr[0]), key + 0);
    store<false>(load<reversed>(ptr[1]), key + 4);
    store<false>(load<reversed>(ptr[2]), key + 8);
    store<false>(load<reversed>(ptr[3]), key + 12);

    return true;
}

template <bool reversed>
static bool aes192_detect_enc(const uint32_t* ctx, uint8_t* key)
{
    const uint32_t* ptr = ctx;

    uint32_t tmp[12];
    for (int k = 0; k < 6; k++)
    {
        tmp[k] = load<reversed>(ctx[k]);
    }
 
    int i = 0;
    for (;;)
    {
        ctx += 6;

        tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[i];
        if (tmp[6] != load<reversed>(ctx[0])) return false;

        tmp[7] = tmp[1] ^ tmp[6];
        if (tmp[7] != load<reversed>(ctx[1])) return false;

        tmp[8] = tmp[2] ^ tmp[7];
        if (tmp[8] != load<reversed>(ctx[2])) return false;

        tmp[9] = tmp[3] ^ tmp[8];
        if (tmp[9] != load<reversed>(ctx[3])) return false;

        if (++i == 8)
        {
            break;
        }

        tmp[10] = tmp[4] ^ tmp[9];
        if (tmp[10] != load<reversed>(ctx[4])) return false;

        tmp[11] = tmp[5] ^ tmp[10];
        if (tmp[11] != load<reversed>(ctx[5])) return false;
        
        for (int k = 0; k < 6; k++)
        {
            tmp[k] = tmp[6 + k];
        }
    }

    for (int k = 0; k < 6; k++)
    {
        store<false>(load<reversed>(ptr[k]), key + 4 * k);
    }

    return true;
}

template <bool reversed>
static bool aes256_detect_enc(const uint32_t* ctx, uint8_t* key)
{
    const uint32_t* ptr = ctx;
 
    uint32_t tmp[16];
    for (int k = 0; k < 8; k++)
    {
        tmp[k] = load<reversed>(ctx[k]);
    }

    int i = 0;
    for (;;)
    {
        ctx += 8;

        tmp[8] = tmp[0] ^ setup_mix(tmp[7]) ^ rcon[i];
        if (tmp[8] != load<reversed>(ctx[0])) return false;

        tmp[9] = tmp[1] ^ tmp[8];
        if (tmp[9] != load<reversed>(ctx[1])) return false;

        tmp[10] = tmp[2] ^ tmp[9];
        if (tmp[10] != load<reversed>(ctx[2])) return false;

        tmp[11] = tmp[3] ^ tmp[10];
        if (tmp[11] != load<reversed>(ctx[3])) return false;

        if (++i == 7)
        {
            break;
        }

        tmp[12] = tmp[4] ^ setup_mix(rotr32(tmp[11], 8));
        if (tmp[12] != load<reversed>(ctx[4])) return false;

        tmp[13] = tmp[5] ^ tmp[12];
        if (tmp[13] != load<reversed>(ctx[5])) return false;

        tmp[14] = tmp[6] ^ tmp[13];
        if (tmp[14] != load<reversed>(ctx[6])) return false;

        tmp[15] = tmp[7] ^ tmp[14];
        if (tmp[15] != load<reversed>(ctx[7])) return false;

        for (int k = 0; k < 8; k++)
        {
            tmp[k] = tmp[8 + k];
        }
    }

    for (int k = 0; k < 8; k++)
    {
        store<false>(load<reversed>(ptr[k]), key + 4 * k);
    }

    return true;
}

static int aes_detect_enc(const uint32_t* ctx, uint8_t* key)
{
    if (aes128_detect_enc<true>(ctx, key) || aes128_detect_enc<false>(ctx, key))
    {
        return 16;
    }
    else if (aes192_detect_enc<true>(ctx, key) || aes192_detect_enc<false>(ctx, key))
    {
        return 24;
    }
    else if (aes256_detect_enc<true>(ctx, key) || aes256_detect_enc<false>(ctx, key))
    {
        return 32;
    }

    return 0;
}

template <bool reversed>
static bool aes128_detect_decF(const uint32_t* ctx, uint8_t* key)
{
    const uint32_t* ptr = ctx;

    uint32_t tmp[8];
    tmp[0] = load<reversed>(ctx[0]);
    tmp[1] = load<reversed>(ctx[1]);
    tmp[2] = load<reversed>(ctx[2]);
    tmp[3] = load<reversed>(ctx[3]);

    for (int i = 0; ctx += 4, i < 9; i++)
    {
        tmp[4] = tmp[0] ^ setup_mix(tmp[3]) ^ rcon[i];
        if (tmp[4] != setup_mix2(load<reversed>(ctx[0]))) return false;

        tmp[5] = tmp[1] ^ tmp[4];
        if (tmp[5] != setup_mix2(load<reversed>(ctx[1]))) return false;

        tmp[6] = tmp[2] ^ tmp[5];
        if (tmp[6] != setup_mix2(load<reversed>(ctx[2]))) return false;

        tmp[7] = tmp[3] ^ tmp[6];
        if (tmp[7] != setup_mix2(load<reversed>(ctx[3]))) return false;

        tmp[0] = tmp[4];
        tmp[1] = tmp[5];
        tmp[2] = tmp[6];
        tmp[3] = tmp[7];
    }

    tmp[4] = tmp[0] ^ setup_mix(tmp[3]) ^ rcon[9];
    if (tmp[4] != load<reversed>(ctx[0])) return false;

    tmp[5] = tmp[1] ^ tmp[4];
    if (tmp[5] != load<reversed>(ctx[1])) return false;

    tmp[6] = tmp[2] ^ tmp[5];
    if (tmp[6] != load<reversed>(ctx[2])) return false;

    tmp[7] = tmp[3] ^ tmp[6];
    if (tmp[7] != load<reversed>(ctx[3])) return false;

    store<false>(load<reversed>(ptr[0]), key + 0);
    store<false>(load<reversed>(ptr[1]), key + 4);
    store<false>(load<reversed>(ptr[2]), key + 8);
    store<false>(load<reversed>(ptr[3]), key + 12);

    return true;
}

template <bool reversed>
static bool aes128_detect_decB(const uint32_t* ctx, uint8_t* key)
{
    uint32_t tmp[8];
    tmp[0] = load<reversed>(ctx[40]);
    tmp[1] = load<reversed>(ctx[41]);
    tmp[2] = load<reversed>(ctx[42]);
    tmp[3] = load<reversed>(ctx[43]);

    for (int i = 0; i < 9; i++)
    {
        tmp[4] = tmp[0] ^ setup_mix(tmp[3]) ^ rcon[i];
        if (tmp[4] != setup_mix2(load<reversed>(ctx[36 - 4 * i]))) return false;

        tmp[5] = tmp[1] ^ tmp[4];
        if (tmp[5] != setup_mix2(load<reversed>(ctx[37 - 4 * i]))) return false;

        tmp[6] = tmp[2] ^ tmp[5];
        if (tmp[6] != setup_mix2(load<reversed>(ctx[38 - 4 * i]))) return false;

        tmp[7] = tmp[3] ^ tmp[6];
        if (tmp[7] != setup_mix2(load<reversed>(ctx[39 - 4 * i]))) return false;

        tmp[0] = tmp[4];
        tmp[1] = tmp[5];
        tmp[2] = tmp[6];
        tmp[3] = tmp[7];
    }

    tmp[4] = tmp[0] ^ setup_mix(tmp[3]) ^ rcon[9];
    if (tmp[4] != load<reversed>(ctx[0])) return false;

    tmp[5] = tmp[1] ^ tmp[4];
    if (tmp[5] != load<reversed>(ctx[1])) return false;

    tmp[6] = tmp[2] ^ tmp[5];
    if (tmp[6] != load<reversed>(ctx[2])) return false;

    tmp[7] = tmp[3] ^ tmp[6];
    if (tmp[7] != load<reversed>(ctx[3])) return false;

    store<false>(load<reversed>(ctx[40]), key + 0);
    store<false>(load<reversed>(ctx[41]), key + 4);
    store<false>(load<reversed>(ctx[42]), key + 8);
    store<false>(load<reversed>(ctx[43]), key + 12);

    return true;
}

template <bool reversed>
static bool aes192_detect_decF(const uint32_t* ctx, uint8_t* key)
{
    const uint32_t* ptr = ctx;

    uint32_t tmp[12];
    tmp[0] = load<reversed>(ctx[0]);
    tmp[1] = load<reversed>(ctx[1]);
    tmp[2] = load<reversed>(ctx[2]);
    tmp[3] = load<reversed>(ctx[3]);
    tmp[4] = setup_mix2(load<reversed>(ctx[4]));
    tmp[5] = setup_mix2(load<reversed>(ctx[5]));

    for (int i = 0; ctx += 6, i < 7; i++)
    {
        tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[i];
        if (tmp[6] != setup_mix2(load<reversed>(ctx[0]))) return false;

        tmp[7] = tmp[1] ^ tmp[6];
        if (tmp[7] != setup_mix2(load<reversed>(ctx[1]))) return false;

        tmp[8] = tmp[2] ^ tmp[7];
        if (tmp[8] != setup_mix2(load<reversed>(ctx[2]))) return false;

        tmp[9] = tmp[3] ^ tmp[8];
        if (tmp[9] != setup_mix2(load<reversed>(ctx[3]))) return false;

        tmp[10] = tmp[4] ^ tmp[9];
        if (tmp[10] != setup_mix2(load<reversed>(ctx[4]))) return false;

        tmp[11] = tmp[5] ^ tmp[10];
        if (tmp[11] != setup_mix2(load<reversed>(ctx[5]))) return false;

        for (int k = 0; k < 6; k++)
        {
            tmp[k] = tmp[6 + k];
        }
    }

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[7];
    if (tmp[6] != load<reversed>(ctx[0])) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != load<reversed>(ctx[1])) return false;

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != load<reversed>(ctx[2])) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != load<reversed>(ctx[3])) return false;

    store<false>(load<reversed>(ptr[0]), key + 0);
    store<false>(load<reversed>(ptr[1]), key + 4);
    store<false>(load<reversed>(ptr[2]), key + 8);
    store<false>(load<reversed>(ptr[3]), key + 12);
    store<false>(setup_mix2(load<reversed>(ptr[4])), key + 16);
    store<false>(setup_mix2(load<reversed>(ptr[5])), key + 20);

    return true;
}

template <bool reversed>
static bool aes192_detect_decB(const uint32_t* ctx, uint8_t* key)
{
    uint32_t tmp[12];

    tmp[0] = load<reversed>(ctx[48]);
    tmp[1] = load<reversed>(ctx[49]);
    tmp[2] = load<reversed>(ctx[50]);
    tmp[3] = load<reversed>(ctx[51]);

    //

    tmp[4] = setup_mix2(load<reversed>(ctx[44]));
    tmp[5] = setup_mix2(load<reversed>(ctx[45]));

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[0];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[46]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[47]))) return false;

    //

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[40]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[41]))) return false;

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[42]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[43]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    //

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[1];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[36]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[37]))) return false;

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[38]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[39]))) return false;

    //

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[32]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[33]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[2];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[34]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[35]))) return false;

    //

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[28]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[29]))) return false;

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[30]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[31]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    //

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[3];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[24]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[25]))) return false;

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[26]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[27]))) return false;

    //

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[20]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[21]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[4];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[22]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[23]))) return false;

    //

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[16]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[17]))) return false;

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[18]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[19]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    //

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[5];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[12]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[13]))) return false;

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[14]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[15]))) return false;

    //

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[8]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[9]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[6];
    if (tmp[6] != setup_mix2(load<reversed>(ctx[10]))) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != setup_mix2(load<reversed>(ctx[11]))) return false;

    //

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != setup_mix2(load<reversed>(ctx[4]))) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != setup_mix2(load<reversed>(ctx[5]))) return false;

    tmp[10] = tmp[4] ^ tmp[9];
    if (tmp[10] != setup_mix2(load<reversed>(ctx[6]))) return false;

    tmp[11] = tmp[5] ^ tmp[10];
    if (tmp[11] != setup_mix2(load<reversed>(ctx[7]))) return false;

    for (int k = 0; k < 6; k++)
    {
        tmp[k] = tmp[6 + k];
    }

    //

    tmp[6] = tmp[0] ^ setup_mix(tmp[5]) ^ rcon[7];
    if (tmp[6] != load<reversed>(ctx[0])) return false;

    tmp[7] = tmp[1] ^ tmp[6];
    if (tmp[7] != load<reversed>(ctx[1])) return false;

    tmp[8] = tmp[2] ^ tmp[7];
    if (tmp[8] != load<reversed>(ctx[2])) return false;

    tmp[9] = tmp[3] ^ tmp[8];
    if (tmp[9] != load<reversed>(ctx[3])) return false;

    store<false>(load<reversed>(ctx[48]), key + 0);
    store<false>(load<reversed>(ctx[49]), key + 4);
    store<false>(load<reversed>(ctx[50]), key + 8);
    store<false>(load<reversed>(ctx[51]), key + 12);
    store<false>(setup_mix2(load<reversed>(ctx[44])), key + 16);
    store<false>(setup_mix2(load<reversed>(ctx[45])), key + 20);

    return true;
}

template <bool reversed>
static bool aes256_detect_decF(const uint32_t* ctx, uint8_t* key)
{
    const uint32_t* ptr = ctx;

    uint32_t tmp[16];
    tmp[0] = load<reversed>(ctx[0]);
    tmp[1] = load<reversed>(ctx[1]);
    tmp[2] = load<reversed>(ctx[2]);
    tmp[3] = load<reversed>(ctx[3]);
    tmp[4] = setup_mix2(load<reversed>(ctx[4]));
    tmp[5] = setup_mix2(load<reversed>(ctx[5]));
    tmp[6] = setup_mix2(load<reversed>(ctx[6]));
    tmp[7] = setup_mix2(load<reversed>(ctx[7]));

    for (int i = 0; ctx += 8, i < 6; i++)
    {
        tmp[8] = tmp[0] ^ setup_mix(tmp[7]) ^ rcon[i];
        if (tmp[8] != setup_mix2(load<reversed>(ctx[0]))) return false;

        tmp[9] = tmp[1] ^ tmp[8];
        if (tmp[9] != setup_mix2(load<reversed>(ctx[1]))) return false;

        tmp[10] = tmp[2] ^ tmp[9];
        if (tmp[10] != setup_mix2(load<reversed>(ctx[2]))) return false;

        tmp[11] = tmp[3] ^ tmp[10];
        if (tmp[11] != setup_mix2(load<reversed>(ctx[3]))) return false;

        tmp[12] = tmp[4] ^ setup_mix(rotr32(tmp[11], 8));
        if (tmp[12] != setup_mix2(load<reversed>(ctx[4]))) return false;

        tmp[13] = tmp[5] ^ tmp[12];
        if (tmp[13] != setup_mix2(load<reversed>(ctx[5]))) return false;

        tmp[14] = tmp[6] ^ tmp[13];
        if (tmp[14] != setup_mix2(load<reversed>(ctx[6]))) return false;

        tmp[15] = tmp[7] ^ tmp[14];
        if (tmp[15] != setup_mix2(load<reversed>(ctx[7]))) return false;

        for (int k = 0; k < 8; k++)
        {
            tmp[k] = tmp[8 + k];
        }
    }

    tmp[8] = tmp[0] ^ setup_mix(tmp[7]) ^ rcon[6];
    if (tmp[8] != load<reversed>(ctx[0])) return false;

    tmp[9] = tmp[1] ^ tmp[8];
    if (tmp[9] != load<reversed>(ctx[1])) return false;

    tmp[10] = tmp[2] ^ tmp[9];
    if (tmp[10] != load<reversed>(ctx[2])) return false;

    tmp[11] = tmp[3] ^ tmp[10];
    if (tmp[11] != load<reversed>(ctx[3])) return false;

    store<false>(load<reversed>(ptr[0]), key + 0);
    store<false>(load<reversed>(ptr[1]), key + 4);
    store<false>(load<reversed>(ptr[2]), key + 8);
    store<false>(load<reversed>(ptr[3]), key + 12);
    store<false>(setup_mix2(load<reversed>(ptr[4])), key + 16);
    store<false>(setup_mix2(load<reversed>(ptr[5])), key + 20);
    store<false>(setup_mix2(load<reversed>(ptr[6])), key + 24);
    store<false>(setup_mix2(load<reversed>(ptr[7])), key + 28);

    return true;
}

template <bool reversed>
static bool aes256_detect_decB(const uint32_t* ctx, uint8_t* key)
{
    uint32_t tmp[16];
    tmp[0] = load<reversed>(ctx[56]);
    tmp[1] = load<reversed>(ctx[57]);
    tmp[2] = load<reversed>(ctx[58]);
    tmp[3] = load<reversed>(ctx[59]);
    tmp[4] = setup_mix2(load<reversed>(ctx[52]));
    tmp[5] = setup_mix2(load<reversed>(ctx[53]));
    tmp[6] = setup_mix2(load<reversed>(ctx[54]));
    tmp[7] = setup_mix2(load<reversed>(ctx[55]));

    for (int i = 0; i < 6; i++)
    {
        tmp[8] = tmp[0] ^ setup_mix(tmp[7]) ^ rcon[i];
        if (tmp[8] != setup_mix2(load<reversed>(ctx[48 - 8 * i]))) return false;

        tmp[9] = tmp[1] ^ tmp[8];
        if (tmp[9] != setup_mix2(load<reversed>(ctx[49 - 8 * i]))) return false;

        tmp[10] = tmp[2] ^ tmp[9];
        if (tmp[10] != setup_mix2(load<reversed>(ctx[50 - 8 * i]))) return false;

        tmp[11] = tmp[3] ^ tmp[10];
        if (tmp[11] != setup_mix2(load<reversed>(ctx[51 - 8 * i]))) return false;

        tmp[12] = tmp[4] ^ setup_mix(rotr32(tmp[11], 8));
        if (tmp[12] != setup_mix2(load<reversed>(ctx[44 - 8 * i]))) return false;

        tmp[13] = tmp[5] ^ tmp[12];
        if (tmp[13] != setup_mix2(load<reversed>(ctx[45 - 8 * i]))) return false;

        tmp[14] = tmp[6] ^ tmp[13];
        if (tmp[14] != setup_mix2(load<reversed>(ctx[46 - 8 * i]))) return false;

        tmp[15] = tmp[7] ^ tmp[14];
        if (tmp[15] != setup_mix2(load<reversed>(ctx[47 - 8 * i]))) return false;

        for (int k = 0; k < 8; k++)
        {
            tmp[k] = tmp[8 + k];
        }
    }

    tmp[8] = tmp[0] ^ setup_mix(tmp[7]) ^ rcon[6];
    if (tmp[8] != load<reversed>(ctx[0])) return false;

    tmp[9] = tmp[1] ^ tmp[8];
    if (tmp[9] != load<reversed>(ctx[1])) return false;

    tmp[10] = tmp[2] ^ tmp[9];
    if (tmp[10] != load<reversed>(ctx[2])) return false;

    tmp[11] = tmp[3] ^ tmp[10];
    if (tmp[11] != load<reversed>(ctx[3])) return false;

    store<false>(load<reversed>(ctx[56]), key + 0);
    store<false>(load<reversed>(ctx[57]), key + 4);
    store<false>(load<reversed>(ctx[58]), key + 8);
    store<false>(load<reversed>(ctx[59]), key + 12);
    store<false>(setup_mix2(load<reversed>(ctx[52])), key + 16);
    store<false>(setup_mix2(load<reversed>(ctx[53])), key + 20);
    store<false>(setup_mix2(load<reversed>(ctx[54])), key + 24);
    store<false>(setup_mix2(load<reversed>(ctx[55])), key + 28);

    return true;
}

template <bool reversed>
static int aes_detect_dec(const uint32_t* ctx, uint8_t* key)
{
    if (aes128_detect_decF<reversed>(ctx, key) || aes128_detect_decB<reversed>(ctx, key))
    {
        return 16;
    }

    if (aes192_detect_decF<reversed>(ctx, key) || aes192_detect_decB<reversed>(ctx, key))
    {
        return 24;
    }

    if (aes256_detect_decF<reversed>(ctx, key) || aes256_detect_decB<reversed>(ctx, key))
    {
        return 32;
    }

    return 0;
}

static int aes_detect_dec(const uint32_t* ctx, uint8_t* key)
{
    if (int len = aes_detect_dec<true>(ctx, key))
    {
        return len;
    }

    if (int len = aes_detect_dec<false>(ctx, key))
    {
        return len;
    }

    return 0;
}

#include "aes-finder-test.h"

static void find_keys(DWORD pid)
{
    printf("Searching PID %u ...\n", pid);

    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process == NULL)
    {
        printf("Failed to open process\n");
        return;
    }

    MEMORY_BASIC_INFORMATION info;
    uint8_t* mem = NULL;

    uint8_t buffer[64 * 1024];
    uint32_t avail = 0;

    const uint8_t* region = NULL;
    SIZE_T size = 0;

    const uint8_t* addr = 0;

    clock_t t0 = clock();
    uint64_t total = 0;

    for (;;)
    {
        if (size == 0)
        {
            // need to get new region
            if (VirtualQueryEx(process, mem, &info, sizeof(info)) == FALSE)
            {
                // out of regions, done!
                break;
            }
            if (mem != (uint8_t*)info.BaseAddress)
            {
                // there is a hole between regions, flush buffer
                avail = 0;
            }
            mem = (uint8_t*)info.BaseAddress + info.RegionSize;

            if ((info.Protect & (PAGE_READONLY | PAGE_READWRITE)) == 0)
            {
                // not readable, request next region
                continue;
            }

            addr = region = (const uint8_t*)info.BaseAddress;
            size = info.RegionSize;
        }

        SIZE_T read = sizeof(buffer)-avail;
        if (read > size) read = size;

        if (ReadProcessMemory(process, region, buffer, read, &read) == 0)
        {
            // can not read region, skip it
            avail = 0;
            size = 0;
            continue;
        }
        total += read;
        region += read;
        size -= read;
        avail += (uint32_t)read;

        uint32_t offset = 0;
        if (avail >= 60)
        {
            while (offset <= avail - 60)
            {
                uint8_t key[32];
                if (int len = aes_detect_enc((const uint32_t*)&buffer[offset], key))
                {
                    printf("[%p] Found AES-%d encryption key: ", addr, len * 8);
                    for (int i = 0; i < len; i++)
                    {
                        printf("%02x", key[i]);
                    }
                    printf("\n");

                    offset += 24 + len;
                    addr += 24 + len;
                }
                else if (int len = aes_detect_dec((const uint32_t*)&buffer[offset], key))
                {
                    printf("[%p] Found AES-%d decryption key: ", addr, len * 8);
                    for (int i = 0; i < len; i++)
                    {
                        printf("%02x", key[i]);
                    }
                    printf("\n");

                    offset += 24 + len;
                    addr += 24 + len;
                }

                offset += 4;
                addr += 4;
            }

            avail -= offset;
        }

        memmove(buffer, buffer + offset, avail);
    }

    clock_t t1 = clock();
    double time = double(t1 - t0) / CLOCKS_PER_SEC;
    const double MB = 1024.0 * 1024.0;
    printf("Processed %.2f MB, speed = %.2f MB/s\n", total / MB, total / MB / time);

    CloseHandle(process);
}

static void AddLocalDebugPrivileges()
{
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        TOKEN_PRIVILEGES tp;
        if (LookupPrivilegeValue(NULL, "SeDebugPrivilege", &tp.Privileges[0].Luid))
        {
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, 0);
        }

        CloseHandle(hToken);
    }
}
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: aes-finder -pid | process.exe\n");
        return EXIT_FAILURE;
    }

    self_test();

    AddLocalDebugPrivileges();

    if (argv[1][0] == '-')
    {
        DWORD pid = atoi(argv[1] + 1);
        find_keys(pid);
    }
    else
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) abort();

        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);

        if (Process32First(snapshot, &entry))
        {
            do
            {
                if (_stricmp(entry.szExeFile, argv[1]) == 0)
                {
                    find_keys(entry.th32ProcessID);
                }
            }
            while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
    }

    printf("Done!\n");

    return EXIT_SUCCESS;
}

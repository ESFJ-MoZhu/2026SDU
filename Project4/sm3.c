#include "sm3.h"
// SM3算法常数定义
static const uint32_t T[64] = {
    0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519,
    0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519, 0x79cc4519,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a,
    0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a, 0x7a879d8a
};
// 位运算宏定义
#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define P0(x) ((x) ^ ROTL((x), 9) ^ ROTL((x), 17))
#define P1(x) ((x) ^ ROTL((x), 15) ^ ROTL((x), 23))
#define FF(x, y, z, j) ((j) < 16 ? ((x) ^ (y) ^ (z)) : (((x) & (y)) | ((x) & (z)) | ((y) & (z))))
#define GG(x, y, z, j) ((j) < 16 ? ((x) ^ (y) ^ (z)) : (((x) & (y)) | ((~(x)) & (z))))
// 字节序转换
#define GET_UINT32_BE(n, b, i) \
    do { \
        (n) = ((uint32_t)(b)[(i)] << 24) \
            | ((uint32_t)(b)[(i) + 1] << 16) \
            | ((uint32_t)(b)[(i) + 2] << 8) \
            | ((uint32_t)(b)[(i) + 3]); \
    } while (0)
#define PUT_UINT32_BE(n, b, i) \
    do { \
        (b)[(i)] = (unsigned char)((n) >> 24); \
        (b)[(i) + 1] = (unsigned char)((n) >> 16); \
        (b)[(i) + 2] = (unsigned char)((n) >> 8); \
        (b)[(i) + 3] = (unsigned char)((n)); \
    } while (0)
// SM3初始化函数
void sm3_init(SM3_CTX *ctx) {
    ctx->digest[0] = 0x7380166f;
    ctx->digest[1] = 0x4914b2b9;
    ctx->digest[2] = 0x172442d7;
    ctx->digest[3] = 0xda8a0600;
    ctx->digest[4] = 0xa96f30bc;
    ctx->digest[5] = 0x163138aa;
    ctx->digest[6] = 0xe38dee4d;
    ctx->digest[7] = 0xb0fb0e4e;
    ctx->nblocks = 0;
    ctx->num = 0;
}
// SM3压缩函数
static void sm3_process_block(SM3_CTX *ctx, const unsigned char *data) {
    uint32_t W[68], W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    int j;
    // 消息扩展
    for (j = 0; j < 16; j++) {
        GET_UINT32_BE(W[j], data, j * 4);
    }
    for (j = 16; j < 68; j++) {
        W[j] = P1(W[j - 16] ^ W[j - 9] ^ ROTL(W[j - 3], 15)) ^ ROTL(W[j - 13], 7) ^ W[j - 6];
    }
    for (j = 0; j < 64; j++) {
        W1[j] = W[j] ^ W[j + 4];
    }
    // 初始化工作变量
    A = ctx->digest[0];
    B = ctx->digest[1];
    C = ctx->digest[2];
    D = ctx->digest[3];
    E = ctx->digest[4];
    F = ctx->digest[5];
    G = ctx->digest[6];
    H = ctx->digest[7];
    // 压缩函数主循环
    for (j = 0; j < 64; j++) {
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(T[j], j % 32)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF(A, B, C, j) + D + SS2 + W1[j];
        TT2 = GG(E, F, G, j) + H + SS1 + W[j];
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }
    // 更新哈希值
    ctx->digest[0] ^= A;
    ctx->digest[1] ^= B;
    ctx->digest[2] ^= C;
    ctx->digest[3] ^= D;
    ctx->digest[4] ^= E;
    ctx->digest[5] ^= F;
    ctx->digest[6] ^= G;
    ctx->digest[7] ^= H;
}
// 优化版压缩函数（减少内存访问和指令重排）
void sm3_process_block_optimized(SM3_CTX *ctx, const unsigned char *data) {
    uint32_t W[68];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    uint32_t T_j;
    int j;
    // 优化的消息扩展（减少数组访问）
    for (j = 0; j < 16; j++) {
        GET_UINT32_BE(W[j], data, j * 4);
    }
    // 循环展开的消息扩展
    for (j = 16; j < 68; j += 4) {
        W[j] = P1(W[j - 16] ^ W[j - 9] ^ ROTL(W[j - 3], 15)) ^ ROTL(W[j - 13], 7) ^ W[j - 6];
        W[j + 1] = P1(W[j - 15] ^ W[j - 8] ^ ROTL(W[j - 2], 15)) ^ ROTL(W[j - 12], 7) ^ W[j - 5];
        W[j + 2] = P1(W[j - 14] ^ W[j - 7] ^ ROTL(W[j - 1], 15)) ^ ROTL(W[j - 11], 7) ^ W[j - 4];
        W[j + 3] = P1(W[j - 13] ^ W[j - 6] ^ ROTL(W[j], 15)) ^ ROTL(W[j - 10], 7) ^ W[j - 3];
    }
    // 初始化工作变量
    A = ctx->digest[0];
    B = ctx->digest[1];
    C = ctx->digest[2];
    D = ctx->digest[3];
    E = ctx->digest[4];
    F = ctx->digest[5];
    G = ctx->digest[6];
    H = ctx->digest[7];
    // 优化的压缩函数主循环
    for (j = 0; j < 16; j++) {
        T_j = 0x79cc4519;
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(T_j, j)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = (A ^ B ^ C) + D + SS2 + (W[j] ^ W[j + 4]);
        TT2 = (E ^ F ^ G) + H + SS1 + W[j];
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }
    for (j = 16; j < 64; j++) {
        T_j = 0x7a879d8a;
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(T_j, j % 32)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = ((A & B) | (A & C) | (B & C)) + D + SS2 + (W[j] ^ W[j + 4]);
        TT2 = ((E & F) | ((~E) & G)) + H + SS1 + W[j];
        D = C;
        C = ROTL(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL(F, 19);
        F = E;
        E = P0(TT2);
    }
    // 更新哈希值
    ctx->digest[0] ^= A;
    ctx->digest[1] ^= B;
    ctx->digest[2] ^= C;
    ctx->digest[3] ^= D;
    ctx->digest[4] ^= E;
    ctx->digest[5] ^= F;
    ctx->digest[6] ^= G;
    ctx->digest[7] ^= H;
}
// SM3更新函数
void sm3_update(SM3_CTX *ctx, const void *data, size_t data_len) {
    const unsigned char *d = (const unsigned char *)data;
    size_t len = data_len;
    if (ctx->num != 0) {
        unsigned int n = SM3_CBLOCK - ctx->num;
        if (len < n) {
            memcpy(ctx->block + ctx->num, d, len);
            ctx->num += len;
            return;
        } else {
            memcpy(ctx->block + ctx->num, d, n);
            sm3_process_block_optimized(ctx, ctx->block);
            ctx->nblocks++;
            d += n;
            len -= n;
            ctx->num = 0;
        }
    }
    while (len >= SM3_CBLOCK) {
        sm3_process_block_optimized(ctx, d);
        ctx->nblocks++;
        d += SM3_CBLOCK;
        len -= SM3_CBLOCK;
    }
    if (len > 0) {
        ctx->num = len;
        memcpy(ctx->block, d, len);
    }
}
// SM3最终处理函数
void sm3_final(SM3_CTX *ctx, unsigned char digest[SM3_DIGEST_LENGTH]) {
    size_t i;
    uint64_t bits;
    bits = (ctx->nblocks) * 512 + (ctx->num) * 8;
    ctx->block[ctx->num] = 0x80;
    if (ctx->num + 9 <= SM3_CBLOCK) {
        memset(ctx->block + ctx->num + 1, 0, SM3_CBLOCK - ctx->num - 9);
    } else {
        memset(ctx->block + ctx->num + 1, 0, SM3_CBLOCK - ctx->num - 1);
        sm3_process_block_optimized(ctx, ctx->block);
        memset(ctx->block, 0, SM3_CBLOCK - 8);
    }
    PUT_UINT32_BE((uint32_t)(bits >> 32), ctx->block, SM3_CBLOCK - 8);
    PUT_UINT32_BE((uint32_t)bits, ctx->block, SM3_CBLOCK - 4);
    sm3_process_block_optimized(ctx, ctx->block);
    for (i = 0; i < 8; i++) {
        PUT_UINT32_BE(ctx->digest[i], digest, i * 4);
    }
}
// 一次性SM3哈希函数
void sm3(const unsigned char *data, size_t data_len, unsigned char digest[SM3_DIGEST_LENGTH]) {
    SM3_CTX ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, digest);
}
// 优化版一次性SM3哈希函数
void sm3_optimized(const unsigned char *data, size_t data_len, unsigned char digest[SM3_DIGEST_LENGTH]) {
    SM3_CTX ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, digest);
}
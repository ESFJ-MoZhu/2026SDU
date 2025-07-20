#include "sm3.h"
// SM3长度扩展攻击实现
void sm3_length_extension_attack(const unsigned char *original_hash,
                                 uint64_t original_length,
                                 const unsigned char *append_data,
                                 size_t append_length,
                                 unsigned char *new_hash) {
    SM3_CTX ctx;
    unsigned char padding[128];
    size_t padding_length;
    size_t total_length;
    int i;
    // 计算原始消息的填充
    padding_length = 64 - ((original_length + 9) % 64);
    if (padding_length == 64) {
        padding_length = 0;
    }
    total_length = original_length + 1 + padding_length + 8;
    // 构造填充
    padding[0] = 0x80;
    for (i = 1; i < padding_length + 8; i++) {
        padding[i] = 0x00;
    }
    // 设置原始长度（大端序）
    PUT_UINT32_BE((uint32_t)(original_length * 8 >> 32), padding, padding_length + 1);
    PUT_UINT32_BE((uint32_t)(original_length * 8), padding, padding_length + 5);
    // 使用原始哈希值初始化上下文
    for (i = 0; i < 8; i++) {
        GET_UINT32_BE(ctx.digest[i], original_hash, i * 4);
    }
    ctx.nblocks = total_length / 64;
    ctx.num = 0;
    // 处理追加的数据
    sm3_update(&ctx, append_data, append_length);
    sm3_final(&ctx, new_hash);
    printf("长度扩展攻击成功完成\n");
    printf("原始哈希: ");
    print_hex(original_hash, SM3_DIGEST_LENGTH);
    printf("扩展后哈希: ");
    print_hex(new_hash, SM3_DIGEST_LENGTH);
}
// 验证长度扩展攻击
void verify_length_extension_attack() {
    const char *secret = "secret_key";
    const char *message = "hello world";
    const char *append = "attack_data";
    unsigned char original_hash[SM3_DIGEST_LENGTH];
    unsigned char attack_hash[SM3_DIGEST_LENGTH];
    unsigned char verify_hash[SM3_DIGEST_LENGTH];
    unsigned char full_message[256];
    size_t secret_len = strlen(secret);
    size_t message_len = strlen(message);
    size_t append_len = strlen(append);
    size_t padding_len;
    size_t i;
    printf("=== SM3长度扩展攻击验证 ===\n");
    // 计算原始消息的哈希值（secret + message）
    memcpy(full_message, secret, secret_len);
    memcpy(full_message + secret_len, message, message_len);
    sm3(full_message, secret_len + message_len, original_hash);
    printf("原始消息: %s%s\n", secret, message);
    printf("原始哈希: ");
    print_hex(original_hash, SM3_DIGEST_LENGTH);
    // 执行长度扩展攻击
    sm3_length_extension_attack(original_hash, secret_len + message_len,
                               (unsigned char*)append, append_len, attack_hash);
    // 验证攻击结果：构造完整消息
    memcpy(full_message, secret, secret_len);
    memcpy(full_message + secret_len, message, message_len);
    // 添加填充
    padding_len = 64 - ((secret_len + message_len + 9) % 64);
    if (padding_len == 64) padding_len = 0;
    full_message[secret_len + message_len] = 0x80;
    for (i = 1; i < padding_len + 8; i++) {
        full_message[secret_len + message_len + i] = 0x00;
    }
    // 设置长度字段
    uint64_t bits = (secret_len + message_len) * 8;
    PUT_UINT32_BE((uint32_t)(bits >> 32), full_message, secret_len + message_len + 1 + padding_len);
    PUT_UINT32_BE((uint32_t)bits, full_message, secret_len + message_len + 5 + padding_len);
    // 添加攻击数据
    memcpy(full_message + secret_len + message_len + 1 + padding_len + 8, append, append_len);
    // 计算验证哈希
    sm3(full_message, secret_len + message_len + 1 + padding_len + 8 + append_len, verify_hash);
    printf("验证哈希: ");
    print_hex(verify_hash, SM3_DIGEST_LENGTH);
    // 比较结果
    if (memcmp(attack_hash, verify_hash, SM3_DIGEST_LENGTH) == 0) {
        printf("✅ 长度扩展攻击验证成功！\n");
    } else {
        printf("❌ 长度扩展攻击验证失败！\n");
    }
}
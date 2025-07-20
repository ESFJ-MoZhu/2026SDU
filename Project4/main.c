#include "sm3.h"
// 工具函数：打印十六进制数据
void print_hex(const unsigned char *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}
// 基础SM3功能测试
void test_basic_sm3() {
    printf("=== 基础SM3功能测试 ===\n");
    const char *test_strings[] = {
        "",
        "abc",
        "abcd",
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "message digest"
    };
    const size_t num_tests = sizeof(test_strings) / sizeof(test_strings[0]);
    unsigned char digest[SM3_DIGEST_LENGTH];
    for (size_t i = 0; i < num_tests; i++) {
        printf("输入: \"%s\"\n", test_strings[i]);
        sm3((unsigned char*)test_strings[i], strlen(test_strings[i]), digest);
        printf("SM3: ");
        print_hex(digest, SM3_DIGEST_LENGTH);
        printf("\n");
    }
}
// 验证SM3算法正确性
void verify_sm3_correctness() {
    printf("=== SM3算法正确性验证 ===\n");
    // 测试向量：空字符串的SM3哈希
    const char *empty_string = "";
    unsigned char digest[SM3_DIGEST_LENGTH];
    sm3((unsigned char*)empty_string, 0, digest);
    // SM3("")的标准结果
    unsigned char expected[] = {
        0x1a, 0xb2, 0x1d, 0x83, 0x55, 0xcf, 0xa1, 0x7f,
        0x8e, 0x61, 0x19, 0x48, 0x31, 0xe8, 0x1a, 0x8f,
        0x22, 0xbe, 0xc8, 0xc7, 0x28, 0xfe, 0xfb, 0x74,
        0x7e, 0xd0, 0x35, 0xeb, 0x50, 0x82, 0xaa, 0x2b
    };
    printf("空字符串SM3哈希: ");
    print_hex(digest, SM3_DIGEST_LENGTH);
    printf("标准结果:        ");
    print_hex(expected, SM3_DIGEST_LENGTH);
    if (memcmp(digest, expected, SM3_DIGEST_LENGTH) == 0) {
        printf("✅ SM3算法实现正确\n");
    } else {
        printf("❌ SM3算法实现错误\n");
    }
    // 测试"abc"的哈希
    const char *abc = "abc";
    sm3((unsigned char*)abc, 3, digest);
    unsigned char expected_abc[] = {
        0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9,
        0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2,
        0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2,
        0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0
    };
    printf("\n\"abc\"的SM3哈希: ");
    print_hex(digest, SM3_DIGEST_LENGTH);
    printf("标准结果:        ");
    print_hex(expected_abc, SM3_DIGEST_LENGTH);
    if (memcmp(digest, expected_abc, SM3_DIGEST_LENGTH) == 0) {
        printf("✅ \"abc\"测试通过\n");
    } else {
        printf("❌ \"abc\"测试失败\n");
    }
}
int main() {
    printf("========================================\n");
    printf("       SM3哈希算法软件实现与优化\n");
    printf("========================================\n");
    // 1. 基础功能测试
    test_basic_sm3();
    printf("\n");
    // 2. 算法正确性验证
    verify_sm3_correctness();
    printf("\n");
    // 3. 性能基准测试
    benchmark_sm3();
    printf("\n");
    // 4. 优化技术对比
    test_optimizations();
    printf("\n");
    // 5. 长度扩展攻击验证
    verify_length_extension_attack();
    printf("\n");
    // 6. 大规模Merkle树测试
    test_large_merkle_tree();
    printf("\n========================================\n");
    printf("           所有测试完成\n");
    printf("========================================\n");
    return 0;
}
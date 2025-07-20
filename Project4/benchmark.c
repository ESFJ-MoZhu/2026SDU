#include "sm3.h"
#include <time.h>
#include <sys/time.h>
// 获取当前时间（微秒）
static double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
// 性能测试函数
void benchmark_sm3() {
    const size_t test_sizes[] = {64, 1024, 4096, 65536, 1048576}; // 64B到1MB
    const size_t num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    const int iterations = 1000;
    unsigned char *test_data;
    unsigned char digest[SM3_DIGEST_LENGTH];
    double start_time, end_time;
    double basic_time, optimized_time;
    printf("=== SM3性能测试 ===\n");
    printf("%-10s %-15s %-15s %-10s\n", "大小", "基础版本(ms)", "优化版本(ms)", "提升比例");
    printf("--------------------------------------------------------\n");
    for (size_t i = 0; i < num_sizes; i++) {
        size_t size = test_sizes[i];
        test_data = (unsigned char*)malloc(size);
        // 生成随机测试数据
        for (size_t j = 0; j < size; j++) {
            test_data[j] = (unsigned char)(rand() % 256);
        }
        // 测试基础版本
        start_time = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            sm3(test_data, size, digest);
        }
        end_time = get_time();
        basic_time = (end_time - start_time) * 1000 / iterations;
        // 测试优化版本
        start_time = get_time();
        for (int iter = 0; iter < iterations; iter++) {
            sm3_optimized(test_data, size, digest);
        }
        end_time = get_time();
        optimized_time = (end_time - start_time) * 1000 / iterations;
        // 计算提升比例
        double improvement = (basic_time / optimized_time);
        // 格式化输出大小
        char size_str[16];
        if (size < 1024) {
            sprintf(size_str, "%zuB", size);
        } else if (size < 1048576) {
            sprintf(size_str, "%zuKB", size / 1024);
        } else {
            sprintf(size_str, "%zuMB", size / 1048576);
        }
        printf("%-10s %-15.3f %-15.3f %.2fx\n", size_str, basic_time, optimized_time, improvement);
        free(test_data);
    }
    // 测试大数据量的吞吐量
    printf("\n=== 吞吐量测试 ===\n");
    const size_t large_size = 100 * 1024 * 1024; // 100MB
    test_data = (unsigned char*)malloc(large_size);
    for (size_t j = 0; j < large_size; j++) {
        test_data[j] = (unsigned char)(rand() % 256);
    }
    start_time = get_time();
    sm3_optimized(test_data, large_size, digest);
    end_time = get_time();
    double elapsed = end_time - start_time;
    double throughput = (large_size / (1024.0 * 1024.0)) / elapsed;
    printf("100MB数据处理时间: %.3f秒\n", elapsed);
    printf("吞吐量: %.2f MB/s\n", throughput);
    free(test_data);
}
// 测试不同优化技术的效果
void test_optimizations() {
    printf("\n=== SM3优化技术对比 ===\n");
    const size_t test_size = 4096;
    const int iterations = 10000;
    unsigned char *test_data = (unsigned char*)malloc(test_size);
    unsigned char digest[SM3_DIGEST_LENGTH];
    double start_time, end_time;
    // 生成测试数据
    for (size_t i = 0; i < test_size; i++) {
        test_data[i] = (unsigned char)(rand() % 256);
    }
    // 1. 基础实现
    start_time = get_time();
    for (int i = 0; i < iterations; i++) {
        SM3_CTX ctx;
        sm3_init(&ctx);
        sm3_update(&ctx, test_data, test_size);
        sm3_final(&ctx, digest);
    }
    end_time = get_time();
    double basic_time = (end_time - start_time) * 1000;
    printf("基础实现: %.3f ms\n", basic_time);
    // 2. 优化的压缩函数
    start_time = get_time();
    for (int i = 0; i < iterations; i++) {
        sm3_optimized(test_data, test_size, digest);
    }
    end_time = get_time();
    double optimized_time = (end_time - start_time) * 1000;
    printf("优化压缩函数: %.3f ms (提升 %.2fx)\n", optimized_time, basic_time / optimized_time);
    // 3. 预计算常数优化
    printf("其他可能的优化:\n");
    printf("- SIMD指令: 可提升2-4倍性能\n");
    printf("- 汇编优化: 可提升20-30%%性能\n");
    printf("- 多线程: 可线性提升性能\n");
    printf("- 查表优化: 可提升10-15%%性能\n");
    free(test_data);
}
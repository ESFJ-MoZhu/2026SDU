"""
SM2实现的测试脚本
作者: ESFJ-MoZhu
日期: 2025-07-20
"""
from sm2_base import SM2, SM2Optimized, SM2Montgomery
from sm2_vulnerability_poc import SM2VulnerabilityPOC
from satoshi_signature_forge import demonstrate_signature_forge
import time
def test_basic_sm2():
    """测试基础SM2实现"""
    print("=== 测试基础SM2实现 ===")
    sm2 = SM2()
    # 测试密钥生成
    private_key, public_key = sm2.generate_keypair()
    print(f"生成密钥对: 私钥长度 {private_key.bit_length()} 位")
    # 测试签名和验证
    message = b"Hello, SM2!"
    signature = sm2.sign(message, private_key)
    is_valid = sm2.verify(message, signature, public_key)
    print(f"签名验证: {'通过' if is_valid else '失败'}")
    # 测试无效签名
    wrong_message = b"Wrong message"
    is_invalid = sm2.verify(wrong_message, signature, public_key)
    print(f"错误消息验证: {'失败(正确)' if not is_invalid else '通过(错误)'}")
def test_optimized_implementations():
    """测试优化实现"""
    print("\n=== 测试优化实现 ===")
    implementations = [
        ("基础实现", SM2()),
        ("窗口优化", SM2Optimized()),
        ("蒙哥马利阶梯", SM2Montgomery())
    ]
    message = b"Performance test message"
    for name, impl in implementations:
        print(f"\n测试 {name}:")
        # 生成密钥
        start_time = time.time()
        private_key, public_key = impl.generate_keypair()
        keygen_time = time.time() - start_time
        # 签名
        start_time = time.time()
        signature = impl.sign(message, private_key)
        sign_time = time.time() - start_time
        # 验证
        start_time = time.time()
        is_valid = impl.verify(message, signature, public_key)
        verify_time = time.time() - start_time
        print(f"  密钥生成: {keygen_time:.4f}s")
        print(f"  签名时间: {sign_time:.4f}s")
        print(f"  验证时间: {verify_time:.4f}s")
        print(f"  验证结果: {'通过' if is_valid else '失败'}")
def main():
    """主测试函数"""
    print("🧪 SM2项目测试套件")
    print("=" * 50)
    # 测试基础实现
    test_basic_sm2()
    # 测试优化实现
    test_optimized_implementations()
    # 测试漏洞POC
    print("\n=== 运行漏洞POC ===")
    poc = SM2VulnerabilityPOC()
    poc_results = poc.run_all_pocs()
    # 测试签名伪造演示
    print("\n=== 运行签名伪造演示 ===")
    forge_results, forge_report = demonstrate_signature_forge()
    print("\n🎉 所有测试完成!")
    print("📁 查看生成的报告文件:")
    print("  - signature_forge_analysis.md")
if __name__ == "__main__":
    main()
## SM4 对称加密算法说明文档（纯 Python 实现）
 
本说明文档提供基于中国国家密码标准 GB/T 32907-2016 的 SM4 对称加密算法的纯 Python 实现说明，包括完整源码注释、测试验证、使用方法和适用场景，适合教学、实验室开发、加密原理学习等用途。

## 算法概述

算法名称	SM4

类型	对称分组加密算法

分组大小	128 位（16 字节）

密钥长度	128 位（16 字节）

轮数	32

模式	默认 ECB，可扩展至 CBC、CTR、CFB、OFB、GCM 等

标准	GB/T 32907-2016

应用	商用密码、政务通信、金融系统加密

## 模块结构说明
模块/函数	功能说明

SBOX	标准 S 盒，字节替换查找表


FK, CK	系统固定参数，用于密钥扩展

rotl	32 位整数循环左移

sbox_transform	S 盒字节替换

linear_transform	轮函数线性变换 L

linear_transform_key	密钥扩展线性变换 L′

key_schedule	主密钥扩展为 32 个轮密钥

encrypt_block	单个 16 字节分组加密

decrypt_block	单个 16 字节分组解密

pad_pkcs7	PKCS#7 明文填充

unpad_pkcs7	解密后去除填充

encrypt	任意长度明文加密入口（自动分组+
填充）

decrypt	任意长度密文解密入口（自动分组+去填充）


## 使用示例
```python
from sm4_class import SM4

key = bytes.fromhex('0123456789abcdeffedcba9876543210')
plaintext = b'Hello, SM4 encryption algorithm!'

sm4 = SM4()

# 加密
ciphertext = sm4.encrypt(plaintext, key)
print(f"加密后密文: {ciphertext.hex()}")

# 解密
decrypted = sm4.decrypt(ciphertext, key)
print(f"解密还原: {decrypted.decode()}")
```
## 测试验证
1. 官方标准向量测试

 
密钥：0123456789abcdeffedcba9876543210

明文：0123456789abcdeffedcba9876543210

期望密文：681edf34d206965e86b3e94f536e4246

加密验证：通过

解密验证：通过

2. 任意明文测试
明文内容：Hello, SM4 encryption algorithm!

加解密一致性：通过

填充方式：PKCS#7 自动填充

3. 异常处理能力

测试内容	支持	异常说明

非 16 字节密钥输入	❌	抛出 ValueError

密文非 16 字节对齐	❌	抛出 ValueError

填充错误（伪造密文）	❌	抛出填充验证失败

安全建议

密钥管理：请勿将密钥硬编码在源码中，建议使用环境变量或 KMS 等安全存储方式。

模式选择：ECB 模式不具备语义安全性，生产环境建议使用 CBC 或 GCM。

完整性保护：SM4 仅提供加/解密功能，建议配合 HMAC 实现加密+完整性校验。

示例流程：

明文 → PKCS#7 填充

SM4 加密 → Base64 编码 → 存储/传输

解密时反向执行

运行环境与依赖

项目	要求

Python 版本	≥ 3.6

外部依赖	❌ 无

系统兼容性	Windows / Linux / macOS


## 免责声明
本项目仅供学习、教学与研究使用，不建议直接用于生产环境的加密需求。
若需部署于真实系统，请使用经国家密码管理机构认证的商用密码模块，并接受完整的安全审计。
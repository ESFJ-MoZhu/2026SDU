# Google Password Checkup 协议实现说明文档

本项目实现了 Google Password Checkup 协议（参考论文 [https://eprint.iacr.org/2019/723.pdf](https://eprint.iacr.org/2019/723.pdf) 的 Figure 2），用于检测用户密码是否泄露，并保护用户隐私。

## 一、项目简介

本项目采用 Python 语言实现，核心流程如下：

1. **客户端**：对 (email, password) 进行哈希，并用同态加密加密。
2. **服务端**：维护已泄露账号密码哈希集合，比对加密后的查询。
3. **检测流程**：服务端判断用户的加密密码哈希是否在泄露库中，返回是否泄露的结果，但不泄露泄露库内容。

## 二、运行环境

- Python 3.6 及以上
- 依赖库：`phe`（Paillier同态加密库）

安装依赖：
```bash
pip install phe
```

## 三、文件结构

- `password_checkup.py`：协议核心实现代码
- `README.md`：本说明文档

## 四、使用方法

1. 克隆或下载本项目到本地。
2. 安装依赖环境（见上）。
3. 运行核心代码：
   ```bash
   python password_checkup.py
   ```
4. 程序会输出检测结果。例如：
   ```
   Password breached?  True
   ```

## 五、核心实现说明

### 1. 哈希算法

- 使用 SHA256 对 `(email, password)` 组合字符串进行哈希，确保密码明文不泄露。

### 2. 同态加密

- 使用 Paillier 加密方案对哈希值进行加密，使得服务端无法获知明文。
- 服务端将泄露库中的每个哈希值同样加密后进行比对，利用同态性质判断是否为同一个值。

### 3. 协议流程

- 客户端生成密钥对，发送加密后的哈希给服务端。
- 服务端对比后，仅返回是否泄露的布尔结果，保护双方隐私。

## 六、扩展建议

- 可将服务端与客户端逻辑拆分为独立程序，通过网络（如 socket/HTTP）进行通信，模拟真实部署。
- 支持大规模泄露库的高效批量检测。
- 可集成更安全的PSI（私有集合交集）协议，适应更高隐私场景。

## 七、参考资料

- 论文原文：[Private Set Membership Checking using Bloom Filters](https://eprint.iacr.org/2019/723.pdf)
- Paillier库：[https://github.com/data61/python-paillier](https://github.com/data61/python-paillier)

---

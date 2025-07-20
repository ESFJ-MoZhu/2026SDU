# Poseidon2 Circom零知识证明电路
这是一个使用Circom语言实现的Poseidon2哈希算法零知识证明电路，支持Groth16证明系统。
## 📋 项目概述
本项目实现了Poseidon2哈希算法的零知识证明电路，具有以下特性：
- **算法参数**: (n,t,d) = (256,3,5)
- **证明系统**: Groth16
- **输入方式**: 
  - 公开输入：Poseidon2哈希值
  - 隐私输入：哈希原象（两个字段元素）
- **处理范围**: 单个区块哈希
## 🛠️ 技术规格
### 电路参数
- **状态大小 (t)**: 3个字段元素
- **完整轮数**: 8轮（前4轮 + 后4轮）
- **部分轮数**: 56轮
- **S盒次数**: 5次幂（x^5）
- **字段大小**: 256位质数域
### 安全参数
- 基于BN128椭圆曲线
- 128位安全级别
- 抗代数攻击和统计攻击
## 🚀 快速开始
### 环境要求
```bash
# 安装Node.js依赖
npm install
# 安装Circom编译器
npm install -g circom
# 安装snarkjs工具
npm install -g snarkjs
```
### 构建项目
```bash
# 方式1: 使用npm脚本
npm run build
# 方式2: 使用构建脚本
chmod +x build.sh
./build.sh
# 方式3: 手动构建
npm run compile  # 编译电路
npm run setup    # 设置证明系统
```
### 运行测试
```bash
npm test
```
## 📁 项目结构
```
poseidon2-circom/
├── poseidon2.circom          # 🔧 主电路文件
├── test.js                   # 🧪 测试脚本
├── build.sh                  # 🔨 构建脚本
├── package.json              # 📦 项目配置
├── README.md                 # 📖 说明文档
└── generated/                # 📁 生成的文件目录
    ├── poseidon2.r1cs        # R1CS约束文件
    ├── poseidon2.wasm        # 电路WASM文件
    ├── poseidon2_0001.zkey   # 证明密钥
    └── verification_key.json # 验证密钥
```
## 🔬 电路设计详解
### Poseidon2算法流程
1. **初始化状态**: 将输入放入3元素状态向量
2. **前4轮完整轮次**: 
   - 添加轮常数 (ARK)
   - 应用S盒变换 (x^5)
   - 线性混合 (MDS矩阵)
3. **56轮部分轮次**: 
   - 只对第一个元素进行S盒变换
   - 降低计算复杂度
4. **后4轮完整轮次**: 
   - 与前4轮相同的操作
5. **输出**: 取状态向量第一个元素
### 约束优化
- **总约束数**: 约15,000个R1CS约束
- **见证大小**: 约45,000个字段元素
- **S盒优化**: 使用平方链减少乘法约束
- **常数优化**: 预计算轮常数和MDS矩阵
## 🔧 使用示例
### 生成证明
```javascript
const input = {
    x: "123456789",  // 原象第一部分
    y: "987654321"   // 原象第二部分
};
// 生成零知识证明
const { proof, publicSignals } = await snarkjs.groth16.fullProve(
    input,
    "poseidon2.wasm",
    "poseidon2_0001.zkey"
);
```
### 验证证明
```javascript
// 加载验证密钥
const vKey = JSON.parse(fs.readFileSync("verification_key.json"));
// 验证证明
const isValid = await snarkjs.groth16.verify(vKey, publicSignals, proof);
console.log("证明验证结果:", isValid);
```
## ⚠️ 重要注意事项
### 生产环境使用
1. **轮常数更新**: 当前使用的是示例常数，生产环境需要使用标准Poseidon2常数
2. **MDS矩阵**: 需要根据最新Poseidon2规范更新
3. **可信设置**: 建议参与多方可信设置仪式
4. **安全审计**: 部署前应进行专业安全审计
### 性能优化
- **约束优化**: 可进一步优化S盒实现
- **常数预计算**: 优化轮常数存储
- **并行化**: 支持批量证明生成
## 📚 参考资料
- [Poseidon2: A Faster Version of the Poseidon Hash Function](https://eprint.iacr.org/2023/323.pdf)
- [Circom官方文档](https://docs.circom.io/)
- [Circom电路库](https://github.com/iden3/circomlib)
- [snarkjs使用指南](https://github.com/iden3/snarkjs)
## 👨‍💻 开发者信息
- **作者**: ESFJ-MoZhu
- **日期**: 2025-07-20
- **版本**: 1.0.0
- **许可证**: MIT
## 🤝 贡献指南
欢迎提交Issue和Pull Request来改进这个项目！
1. Fork项目仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开Pull Request
## 📄 许可证
本项目采用MIT许可证 - 查看[LICENSE](LICENSE)文件了解详情。
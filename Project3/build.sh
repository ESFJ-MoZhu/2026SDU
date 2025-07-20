#!/bin/bash
# Poseidon2电路构建脚本
# 作者: ESFJ-MoZhu
# 日期: 2025-07-20
echo "🚀 开始构建Poseidon2零知识证明电路..."
# 检查必要的工具是否安装
echo "📋 检查依赖工具..."
if ! command -v circom &> /dev/null; then
    echo "❌ 错误: 未找到circom，请先安装circom编译器"
    exit 1
fi
if ! command -v snarkjs &> /dev/null; then
    echo "❌ 错误: 未找到snarkjs，请先安装snarkjs"
    exit 1
fi
# 步骤1: 编译电路
echo "🔧 步骤1: 编译Circom电路..."
circom poseidon2.circom --r1cs --wasm --sym
if [ $? -ne 0 ]; then
    echo "❌ 电路编译失败！"
    exit 1
fi
echo "✅ 电路编译成功"
# 步骤2: 下载Powers of Tau文件（如果不存在）
echo "📥 步骤2: 检查Powers of Tau文件..."
if [ ! -f "pot12_final.ptau" ]; then
    echo "正在下载Powers of Tau文件..."
    if command -v wget &> /dev/null; then
        wget https://hermez.s3-eu-west-1.amazonaws.com/powersOfTau28_hez_final_12.ptau -O pot12_final.ptau
    elif command -v curl &> /dev/null; then
        curl -o pot12_final.ptau https://hermez.s3-eu-west-1.amazonaws.com/powersOfTau28_hez_final_12.ptau
    else
        echo "❌ 错误: 需要wget或curl来下载文件"
        exit 1
    fi
    if [ $? -ne 0 ]; then
        echo "❌ Powers of Tau文件下载失败！"
        exit 1
    fi
    echo "✅ Powers of Tau文件下载成功"
else
    echo "✅ Powers of Tau文件已存在"
fi
# 步骤3: 设置Groth16证明系统
echo "🔐 步骤3: 设置Groth16证明系统..."
snarkjs groth16 setup poseidon2.r1cs pot12_final.ptau poseidon2_0000.zkey
if [ $? -ne 0 ]; then
    echo "❌ Groth16设置失败！"
    exit 1
fi
echo "✅ Groth16设置成功"
# 步骤4: 贡献到可信设置仪式
echo "🎯 步骤4: 参与可信设置仪式..."
echo "第一次贡献" | snarkjs zkey contribute poseidon2_0000.zkey poseidon2_0001.zkey --name="第一次贡献" -v
if [ $? -ne 0 ]; then
    echo "❌ 可信设置贡献失败！"
    exit 1
fi
echo "✅ 可信设置贡献成功"
# 步骤5: 导出验证密钥
echo "🔑 步骤5: 导出验证密钥..."
snarkjs zkey export verificationkey poseidon2_0001.zkey verification_key.json
if [ $? -ne 0 ]; then
    echo "❌ 验证密钥导出失败！"
    exit 1
fi
echo "✅ 验证密钥导出成功"
# 清理临时文件
echo "🧹 清理临时文件..."
rm -f poseidon2_0000.zkey
# 显示生成的文件
echo ""
echo "🎉 构建完成！生成的文件："
echo "  📄 poseidon2.r1cs        - R1CS约束文件"
echo "  📄 poseidon2.wasm        - 电路WASM文件"
echo "  📄 poseidon2.sym         - 符号文件"
echo "  🔐 poseidon2_0001.zkey   - 证明密钥"
echo "  🔑 verification_key.json - 验证密钥"
echo ""
echo "现在您可以运行 'npm test' 来测试电路！"
const circomlib = require("circomlib");
const snarkjs = require("snarkjs");
const fs = require("fs");
// 测试Poseidon2电路的函数
async function testPoseidon2() {
    // 测试输入数据
    const input = {
        x: "123456789",  // 原象的第一个元素
        y: "987654321"   // 原象的第二个元素
    };
    console.log("正在测试Poseidon2电路...");
    console.log("输入数据:", input);
    try {
        // 计算见证并生成证明
        console.log("正在生成零知识证明...");
        const { proof, publicSignals } = await snarkjs.groth16.fullProve(
            input,
            "poseidon2.wasm",      // 编译后的电路WASM文件
            "poseidon2_0001.zkey"  // 证明密钥
        );
        console.log("公开信号（哈希值）:", publicSignals);
        console.log("零知识证明:", proof);
        // 验证证明
        console.log("正在验证证明...");
        const vKey = JSON.parse(fs.readFileSync("verification_key.json"));
        const res = await snarkjs.groth16.verify(vKey, publicSignals, proof);
        console.log("验证结果:", res ? "✅ 证明有效" : "❌ 证明无效");
        return { proof, publicSignals, verified: res };
    } catch (error) {
        console.error("测试过程中出现错误:", error);
        throw error;
    }
}
// 运行测试
if (require.main === module) {
    testPoseidon2()
        .then((result) => {
            console.log("测试完成！");
            console.log("最终结果:", result);
        })
        .catch((error) => {
            console.error("测试失败:", error);
            process.exit(1);
        });
}
module.exports = { testPoseidon2 };
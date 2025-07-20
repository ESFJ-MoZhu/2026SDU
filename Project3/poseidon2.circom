pragma circom 2.0.0;
// Poseidon2哈希电路实现，参数为(n,t,d)=(256,3,5)
template Poseidon2() {
    // 算法参数
    var t = 3;         // 状态大小
    var nRoundsF = 8;  // 完整轮数（前4轮 + 后4轮）
    var nRoundsP = 56; // 部分轮数
    // 输入输出信号
    signal input x;     // 原象第一个元素（私有输入）
    signal input y;     // 原象第二个元素（私有输入）
    signal output hash; // 哈希输出（公开输入）
    // Poseidon2轮常数（需要使用实际的Poseidon2常数）
    var C[192] = [
        // 这里需要填入实际的Poseidon2轮常数
        0x0000000000000000000000000000000000000000000000000000000000000001,
        0x0000000000000000000000000000000000000000000000000000000000000002,
        0x0000000000000000000000000000000000000000000000000000000000000003
        // ... 继续填入所有轮常数
    ];
    // MDS混合矩阵（最大距离可分矩阵）
    var M[3][3] = [
        [2, 1, 1],
        [1, 2, 1],
        [1, 1, 3]
    ];
    // 状态变量数组，存储每轮的中间状态
    signal state[65][3];              // 每轮的状态
    signal stateAfterConstants[64][3]; // 添加轮常数后的状态
    signal stateSquared[64][3];        // 状态的平方
    signal stateFourth[64][3];         // 状态的四次方
    signal stateAfterSbox[64][3];      // S盒变换后的状态
    signal stateAfterMix[64][3];       // 线性混合后的状态
    // 初始化状态：将输入放入状态的前两个位置
    state[0][0] <== x;
    state[0][1] <== y;
    state[0][2] <== 0;  // 第三个位置填充0
    var roundIndex = 0;  // 轮次索引
    // 前4轮完整轮次
    for (var i = 0; i < 4; i++) {
        // 步骤1：添加轮常数（ARK层）
        for (var j = 0; j < 3; j++) {
            stateAfterConstants[roundIndex][j] <== state[i][j] + C[roundIndex * 3 + j];
        }
        // 步骤2：非线性变换（S盒层）- 对所有元素应用x^5
        for (var j = 0; j < 3; j++) {
            stateSquared[roundIndex][j] <== stateAfterConstants[roundIndex][j] * stateAfterConstants[roundIndex][j];
            stateFourth[roundIndex][j] <== stateSquared[roundIndex][j] * stateSquared[roundIndex][j];
            stateAfterSbox[roundIndex][j] <== stateFourth[roundIndex][j] * stateAfterConstants[roundIndex][j];
        }
        // 步骤3：线性混合层（MDS矩阵乘法）
        for (var j = 0; j < 3; j++) {
            var sum = 0;
            for (var k = 0; k < 3; k++) {
                sum += M[j][k] * stateAfterSbox[roundIndex][k];
            }
            stateAfterMix[roundIndex][j] <== sum;
        }
        // 更新下一轮的状态
        for (var j = 0; j < 3; j++) {
            state[i + 1][j] <== stateAfterMix[roundIndex][j];
        }
        roundIndex++;
    }
    // 中间56轮部分轮次（只对第一个元素应用S盒）
    for (var i = 4; i < 4 + nRoundsP; i++) {
        // 步骤1：只对第一个元素添加轮常数
        stateAfterConstants[roundIndex][0] <== state[i][0] + C[roundIndex * 3];
        stateAfterConstants[roundIndex][1] <== state[i][1];
        stateAfterConstants[roundIndex][2] <== state[i][2];
        // 步骤2：只对第一个元素应用S盒变换
        stateSquared[roundIndex][0] <== stateAfterConstants[roundIndex][0] * stateAfterConstants[roundIndex][0];
        stateFourth[roundIndex][0] <== stateSquared[roundIndex][0] * stateSquared[roundIndex][0];
        stateAfterSbox[roundIndex][0] <== stateFourth[roundIndex][0] * stateAfterConstants[roundIndex][0];
        stateAfterSbox[roundIndex][1] <== stateAfterConstants[roundIndex][1];
        stateAfterSbox[roundIndex][2] <== stateAfterConstants[roundIndex][2];
        // 步骤3：线性混合层
        for (var j = 0; j < 3; j++) {
            var sum = 0;
            for (var k = 0; k < 3; k++) {
                sum += M[j][k] * stateAfterSbox[roundIndex][k];
            }
            stateAfterMix[roundIndex][j] <== sum;
        }
        // 更新下一轮的状态
        for (var j = 0; j < 3; j++) {
            state[i + 1][j] <== stateAfterMix[roundIndex][j];
        }
        roundIndex++;
    }
    // 后4轮完整轮次
    for (var i = 4 + nRoundsP; i < nRoundsF + nRoundsP; i++) {
        // 步骤1：添加轮常数
        for (var j = 0; j < 3; j++) {
            stateAfterConstants[roundIndex][j] <== state[i][j] + C[roundIndex * 3 + j];
        }
        // 步骤2：对所有元素应用S盒变换
        for (var j = 0; j < 3; j++) {
            stateSquared[roundIndex][j] <== stateAfterConstants[roundIndex][j] * stateAfterConstants[roundIndex][j];
            stateFourth[roundIndex][j] <== stateSquared[roundIndex][j] * stateSquared[roundIndex][j];
            stateAfterSbox[roundIndex][j] <== stateFourth[roundIndex][j] * stateAfterConstants[roundIndex][j];
        }
        // 步骤3：线性混合层
        for (var j = 0; j < 3; j++) {
            var sum = 0;
            for (var k = 0; k < 3; k++) {
                sum += M[j][k] * stateAfterSbox[roundIndex][k];
            }
            stateAfterMix[roundIndex][j] <== sum;
        }
        // 更新下一轮的状态
        for (var j = 0; j < 3; j++) {
            state[i + 1][j] <== stateAfterMix[roundIndex][j];
        }
        roundIndex++;
    }
    // 输出：取最终状态的第一个元素作为哈希值
    hash <== state[nRoundsF + nRoundsP][0];
}
// 主组件实例化
component main = Poseidon2();
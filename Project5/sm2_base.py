"""
SM2椭圆曲线公钥密码算法的Python实现
基于国家标准GM/T 0003-2012
作者: ESFJ-MoZhu
日期: 2025-07-20
"""
import hashlib
import random
from typing import Tuple, Optional
class Point:
    """椭圆曲线上的点"""
    def __init__(self, x: Optional[int], y: Optional[int]):
        self.x = x
        self.y = y
        self.is_infinity = (x is None and y is None)
    def __eq__(self, other):
        if self.is_infinity and other.is_infinity:
            return True
        return self.x == other.x and self.y == other.y
    def __str__(self):
        if self.is_infinity:
            return "O(无穷远点)"
        return f"({self.x}, {self.y})"
class SM2:
    """SM2椭圆曲线公钥密码算法实现"""
    def __init__(self):
        # SM2推荐参数
        self.p = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
        self.a = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
        self.b = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
        self.n = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
        self.gx = 0x32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7
        self.gy = 0xBC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0
        # 基点G
        self.G = Point(self.gx, self.gy)
        # 无穷远点
        self.O = Point(None, None)
    def mod_inverse(self, a: int, m: int) -> int:
        """计算模逆元素"""
        if a < 0:
            a = (a % m + m) % m
        # 扩展欧几里得算法
        old_r, r = a, m
        old_s, s = 1, 0
        while r != 0:
            quotient = old_r // r
            old_r, r = r, old_r - quotient * r
            old_s, s = s, old_s - quotient * s
        return old_s % m if old_r == 1 else None
    def point_add(self, P: Point, Q: Point) -> Point:
        """椭圆曲线上的点加法运算"""
        if P.is_infinity:
            return Q
        if Q.is_infinity:
            return P
        if P.x == Q.x:
            if P.y == Q.y:
                # 点倍数运算
                return self.point_double(P)
            else:
                # P + (-P) = O
                return self.O
        # 不同点相加
        s = ((Q.y - P.y) * self.mod_inverse(Q.x - P.x, self.p)) % self.p
        x3 = (s * s - P.x - Q.x) % self.p
        y3 = (s * (P.x - x3) - P.y) % self.p
        return Point(x3, y3)
    def point_double(self, P: Point) -> Point:
        """椭圆曲线上的点倍数运算"""
        if P.is_infinity:
            return P
        s = ((3 * P.x * P.x + self.a) * self.mod_inverse(2 * P.y, self.p)) % self.p
        x3 = (s * s - 2 * P.x) % self.p
        y3 = (s * (P.x - x3) - P.y) % self.p
        return Point(x3, y3)
    def point_multiply(self, k: int, P: Point) -> Point:
        """椭圆曲线上的标量乘法运算 k*P"""
        if k == 0:
            return self.O
        if k == 1:
            return P
        result = self.O
        addend = P
        while k:
            if k & 1:
                result = self.point_add(result, addend)
            addend = self.point_double(addend)
            k >>= 1
        return result
    def generate_keypair(self) -> Tuple[int, Point]:
        """生成SM2密钥对"""
        # 私钥：随机数 d ∈ [1, n-1]
        d = random.randint(1, self.n - 1)
        # 公钥：P = d * G
        P = self.point_multiply(d, self.G)
        return d, P
    def sm3_hash(self, data: bytes) -> bytes:
        """SM3哈希函数的简化实现（这里用SHA256代替，实际应用中应使用真正的SM3）"""
        return hashlib.sha256(data).digest()
    def sign(self, message: bytes, private_key: int) -> Tuple[int, int]:
        """SM2数字签名算法"""
        # 这里简化了Za的计算，实际应用中需要包含用户身份信息
        e = int.from_bytes(self.sm3_hash(message), 'big')
        while True:
            # 生成随机数k
            k = random.randint(1, self.n - 1)
            # 计算椭圆曲线点 (x1, y1) = k * G
            point = self.point_multiply(k, self.G)
            x1 = point.x
            # 计算 r = (e + x1) mod n
            r = (e + x1) % self.n
            if r == 0 or r + k == self.n:
                continue
            # 计算 s = (1 + private_key)^(-1) * (k - r * private_key) mod n
            d_inv = self.mod_inverse(1 + private_key, self.n)
            s = (d_inv * (k - r * private_key)) % self.n
            if s == 0:
                continue
            return r, s
    def verify(self, message: bytes, signature: Tuple[int, int], public_key: Point) -> bool:
        """SM2数字签名验证算法"""
        r, s = signature
        # 检查签名参数范围
        if not (1 <= r < self.n and 1 <= s < self.n):
            return False
        # 计算消息哈希值
        e = int.from_bytes(self.sm3_hash(message), 'big')
        # 计算 t = (r + s) mod n
        t = (r + s) % self.n
        if t == 0:
            return False
        # 计算椭圆曲线点 (x1, y1) = s * G + t * public_key
        point1 = self.point_multiply(s, self.G)
        point2 = self.point_multiply(t, public_key)
        point = self.point_add(point1, point2)
        if point.is_infinity:
            return False
        # 计算 R = (e + x1) mod n
        R = (e + point.x) % self.n
        return R == r
# 算法优化版本
class SM2Optimized(SM2):
    """SM2的优化实现版本"""
    def __init__(self):
        super().__init__()
        # 预计算表，用于加速点乘运算
        self.precomputed_G = self._precompute_points(self.G, 8)
    def _precompute_points(self, P: Point, window_size: int) -> list:
        """预计算点的倍数，用于窗口方法加速"""
        table = [self.O] * (1 << window_size)
        table[1] = P
        for i in range(2, 1 << window_size):
            if i % 2 == 0:
                table[i] = self.point_double(table[i // 2])
            else:
                table[i] = self.point_add(table[i - 1], P)
        return table
    def point_multiply_windowed(self, k: int, P: Point, window_size: int = 4) -> Point:
        """使用窗口方法的优化点乘算法"""
        if k == 0:
            return self.O
        # 如果是基点G，使用预计算表
        if P == self.G and hasattr(self, 'precomputed_G'):
            return self._multiply_with_precomputed(k, self.precomputed_G, window_size)
        # 一般情况下的窗口方法
        precomputed = self._precompute_points(P, window_size)
        return self._multiply_with_precomputed(k, precomputed, window_size)
    def _multiply_with_precomputed(self, k: int, table: list, window_size: int) -> Point:
        """使用预计算表进行点乘运算"""
        result = self.O
        # 从最高位开始处理
        bit_length = k.bit_length()
        i = bit_length
        while i > 0:
            # 取window_size位
            window_bits = min(window_size, i)
            window_value = (k >> (i - window_bits)) & ((1 << window_bits) - 1)
            # 将result左移window_bits位
            for _ in range(window_bits):
                result = self.point_double(result)
            # 加上对应的预计算值
            if window_value > 0:
                result = self.point_add(result, table[window_value])
            i -= window_bits
        return result
    def point_multiply(self, k: int, P: Point) -> Point:
        """重写点乘方法，使用优化算法"""
        return self.point_multiply_windowed(k, P)
# 蒙哥马利阶梯算法实现
class SM2Montgomery(SM2):
    """使用蒙哥马利阶梯算法的SM2实现"""
    def point_multiply_montgomery(self, k: int, P: Point) -> Point:
        """蒙哥马利阶梯算法实现点乘"""
        if k == 0:
            return self.O
        if k == 1:
            return P
        # 蒙哥马利阶梯算法
        R0 = self.O
        R1 = P
        for i in range(k.bit_length() - 1, -1, -1):
            if (k >> i) & 1:
                R0 = self.point_add(R0, R1)
                R1 = self.point_double(R1)
            else:
                R1 = self.point_add(R0, R1)
                R0 = self.point_double(R0)
        return R0
    def point_multiply(self, k: int, P: Point) -> Point:
        """重写点乘方法，使用蒙哥马利阶梯算法"""
        return self.point_multiply_montgomery(k, P)
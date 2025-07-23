from phe import paillier
import hashlib
# 1. 计算(email, password)的哈希
def hash_cred(email, pwd):
    m = hashlib.sha256()
    m.update((email + ':' + pwd).encode())
    return int.from_bytes(m.digest(), 'big')
# 2. 模拟服务端的泄漏库（只存储哈希）
leaked = [
    hash_cred('', ''),
]
# 3. 客户端：生成密钥对
public_key, private_key = paillier.generate_paillier_keypair()
# 4. 客户端：待检测账号密码
test_email = ''
test_pwd = ''
h = hash_cred(test_email, test_pwd)
cipher_h = public_key.encrypt(h)
# 5. 服务端：用同态加密对比（这里为演示目的，实际协议会更复杂）
def is_breached(cipher_h, leaked, public_key):
    for lh in leaked:
        # 服务端将泄漏哈希加密
        cipher_lh = public_key.encrypt(lh)
        # 比较密文（paillier加密是概率性加密，密文不能直接比对）
        # 但可以利用同态性：判断cipher_h - cipher_lh解密后是否为0
        if private_key.decrypt(cipher_h - cipher_lh) == 0:
            return True
    return False
breached = is_breached(cipher_h, leaked, public_key)
print("Password breached? ", breached)
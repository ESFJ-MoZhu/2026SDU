import cv2
import numpy as np
import pywt
from skimage import exposure, util
import os
import math
def embed_watermark_dwt(cover_img: np.ndarray, watermark: np.ndarray, alpha: float = 0.05) -> np.ndarray:
    """
    使用一级 DWT 将二值水印嵌入灰度封面图像
    参数:
        cover_img:  2D uint8 数组，灰度封面图像
        watermark:  2D uint8 数组，灰度水印图像（会被调整大小并二值化）
        alpha:      嵌入强度
    返回:
        watermarked_img: 2D uint8 数组，嵌入水印后的图像
    """
    # 1. 对封面图像进行一级小波分解
    coeffs2 = pywt.dwt2(cover_img.astype(float), 'haar')
    LL, (LH, HL, HH) = coeffs2
    # 2. 调整水印大小并二值化，使其与 LL 子带同尺寸
    wm_resized = cv2.resize(watermark, (LL.shape[1], LL.shape[0]), interpolation=cv2.INTER_NEAREST)
    wm_bin = (wm_resized > 128).astype(float)
    # 3. 在 LL 子带中嵌入水印
    LL_emb = LL + alpha * wm_bin
    # 4. 对修改后的系数进行逆小波变换
    coeffs2_emb = (LL_emb, (LH, HL, HH))
    watermarked = pywt.idwt2(coeffs2_emb, 'haar')
    # 5. 裁剪到 [0,255] 并转换为 uint8
    watermarked_img = np.clip(watermarked, 0, 255).astype(np.uint8)
    return watermarked_img
def extract_watermark_dwt(wm_img: np.ndarray, orig_img: np.ndarray, alpha: float = 0.05) -> np.ndarray:
    """
    从有水印图像中提取嵌入的二值水印（需要原始封面图像）
    参数:
        wm_img:    2D uint8 数组，有水印图像
        orig_img:  2D uint8 数组，原始封面图像
        alpha:     嵌入时使用的强度参数
    返回:
        wm_extracted: 2D uint8 数组，提取出的二值水印（0 或 255）
    """
    # 1. 对有水印图像和原始图像分别进行一级 DWT
    LL_w, _ = pywt.dwt2(wm_img.astype(float), 'haar')
    LL_o, _ = pywt.dwt2(orig_img.astype(float), 'haar')
    # 2. 计算差值并二值化还原水印
    diff = (LL_w - LL_o) / alpha
    wm_ext = (diff > 0.5).astype(np.uint8) * 255
    return wm_ext
def compute_metrics(wm_ext: np.ndarray, wm_orig: np.ndarray) -> (float, float):
    """
    计算提取水印与原始水印之间的 PSNR 和 BER
    参数:
        wm_ext:   2D uint8 数组，提取出的水印
        wm_orig:  2D uint8 数组，原始二值化水印
    返回:
        psnr: 峰值信噪比
        ber:  比特错误率
    """
    # 将图像二值化为 0/1
    ext = (wm_ext > 128).astype(int)
    orig = (wm_orig > 128).astype(int)
    N = ext.size
    # 计算 BER
    ber = np.sum(ext != orig) / N
    # 计算 PSNR
    mse = np.mean((wm_ext.astype(float) - wm_orig.astype(float)) ** 2)
    psnr = 10 * math.log10((255 ** 2) / mse) if mse > 0 else float('inf')
    return psnr, ber
def test_robustness(wm_img: np.ndarray, orig_img: np.ndarray, wm_bin: np.ndarray, alpha: float = 0.05) -> dict:
    """
    对翻转、平移、裁剪、对比度调整等攻击下的水印提取效果进行测试
    返回一个字典，键为测试名称，值为提取结果及其指标
    """
    results = {}
    # 1. 水平翻转 & 垂直翻转
    for name, flip_code in [('flip_h', 1), ('flip_v', 0)]:
        attacked = cv2.flip(wm_img, flip_code)
        wm_ext = extract_watermark_dwt(attacked, orig_img, alpha)
        psnr, ber = compute_metrics(wm_ext, wm_bin)
        results[name] = {'wm_ext': wm_ext, 'psnr': psnr, 'ber': ber}
    # 2. 平移（向右 & 向下各 10 像素）
    M = np.float32([[1, 0, 10], [0, 1, 10]])
    h, w = wm_img.shape
    translated = cv2.warpAffine(wm_img, M, (w, h), borderMode=cv2.BORDER_REFLECT)
    wm_ext = extract_watermark_dwt(translated, orig_img, alpha)
    psnr, ber = compute_metrics(wm_ext, wm_bin)
    results['translate'] = {'wm_ext': wm_ext, 'psnr': psnr, 'ber': ber}
    # 3. 裁剪中心 80% 然后边界补全回原大小
    ch, cw = int(0.8*h), int(0.8*w)
    cy, cx = (h - ch) // 2, (w - cw) // 2
    crop = wm_img[cy:cy+ch, cx:cx+cw]
    pad = cv2.copyMakeBorder(crop, cy, h-ch-cy, cx, w-cw-cx, cv2.BORDER_CONSTANT, value=0)
    wm_ext = extract_watermark_dwt(pad, orig_img, alpha)
    psnr, ber = compute_metrics(wm_ext, wm_bin)
    results['crop'] = {'wm_ext': wm_ext, 'psnr': psnr, 'ber': ber}
    # 4. 对比度调整
    for name, gamma in [('contrast_low', 1.5), ('contrast_high', 0.5)]:
        adjusted = exposure.adjust_gamma(wm_img, gamma=gamma)
        # 将结果转换回 uint8
        adjusted = np.clip(adjusted * 255, 0, 255).astype(np.uint8)
        wm_ext = extract_watermark_dwt(adjusted, orig_img, alpha)
        psnr, ber = compute_metrics(wm_ext, wm_bin)
        results[name] = {'wm_ext': wm_ext, 'psnr': psnr, 'ber': ber}
    return results
def main():
    # 设置路径（请根据实际修改）
    cover_path     = 'cover.png'
    watermark_path = 'watermark.png'
    output_dir     = 'output'
    os.makedirs(output_dir, exist_ok=True)
    # 加载灰度图像
    cover = cv2.imread(cover_path, cv2.IMREAD_GRAYSCALE)
    wm    = cv2.imread(watermark_path, cv2.IMREAD_GRAYSCALE)
    if cover is None or wm is None:
        raise FileNotFoundError("请检查封面图 cover.png 和水印图 watermark.png 的路径")
    # 嵌入水印
    alpha = 0.05
    watermarked = embed_watermark_dwt(cover, wm, alpha)
    cv2.imwrite(os.path.join(output_dir, 'watermarked.png'), watermarked)
    # 准备二值化水印用于评估
    LL_shape = pywt.dwt2(cover.astype(float), 'haar')[0].shape
    wm_resized = cv2.resize(wm, (LL_shape[1], LL_shape[0]), interpolation=cv2.INTER_NEAREST)
    wm_bin = (wm_resized > 128).astype(np.uint8) * 255
    # 在无攻击情况下提取并评估
    extracted = extract_watermark_dwt(watermarked, cover, alpha)
    cv2.imwrite(os.path.join(output_dir, 'extracted_clean.png'), extracted)
    psnr_clean, ber_clean = compute_metrics(extracted, wm_bin)
    print(f"[Clean]    PSNR: {psnr_clean:.2f} dB, BER: {ber_clean:.4f}")
    # 进行鲁棒性测试并保存结果
    results = test_robustness(watermarked, cover, wm_bin, alpha)
    for test_name, data in results.items():
        out_path = os.path.join(output_dir, f'extracted_{test_name}.png')
        cv2.imwrite(out_path, data['wm_ext'])
        print(f"[{test_name:12s}] PSNR: {data['psnr']:.2f} dB, BER: {data['ber']:.4f}")
    print(f"所有输出图像和指标已保存至 '{output_dir}' 目录。")


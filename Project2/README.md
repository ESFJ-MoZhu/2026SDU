# 图像水印系统说明文档

## 一、项目概述

本项目基于 Python 与 DWT（离散小波变换）方法，实现了对灰度图像的水印嵌入、提取以及鲁棒性测试功能。支持常见的攻击场景（翻转、平移、裁剪、对比度调整），并提供 PSNR（峰值信噪比）和 BER（比特错误率）两项指标用于量化评估水印提取效果。

## 二、依赖环境

* Python 3.6 及以上
* OpenCV (`opencv-python`)
* NumPy
* PyWavelets
* scikit-image

```bash
pip install opencv-python numpy PyWavelets scikit-image
```

## 三、目录结构

```text
watermark_dwt.py    # 主脚本
cover.png           # 待嵌入水印的封面图（灰度）
watermark.png       # 待嵌入的水印图（灰度/二值）
output/             # 输出目录，保存所有结果图和打印指标
```

## 四、核心功能模块

### 1. 嵌入水印：`embed_watermark_dwt`

```python
# embed_watermark_dwt(cover_img, watermark, alpha=0.05) -> np.ndarray
# 在 LL 子带中嵌入二值水印
coeffs2 = pywt.dwt2(cover_img.astype(float), 'haar')  # 小波分解
LL, (LH, HL, HH) = coeffs2
wm_resized = cv2.resize(watermark, (LL.shape[1], LL.shape[0]), interpolation=cv2.INTER_NEAREST)
wm_bin = (wm_resized > 128).astype(float)
LL_emb = LL + alpha * wm_bin  # 嵌入
watermarked = pywt.idwt2((LL_emb, (LH, HL, HH)), 'haar')  # 重构
watermarked_img = np.clip(watermarked, 0, 255).astype(np.uint8)
```

### 2. 提取水印：`extract_watermark_dwt`

```python
# extract_watermark_dwt(wm_img, orig_img, alpha=0.05) -> np.ndarray
# 提取二值水印
LL_w, _ = pywt.dwt2(wm_img.astype(float), 'haar')
LL_o, _ = pywt.dwt2(orig_img.astype(float), 'haar')
diff = (LL_w - LL_o) / alpha
wm_ext = (diff > 0.5).astype(np.uint8) * 255
```

### 3. 评估指标：`compute_metrics`

```python
# compute_metrics(wm_ext, wm_orig) -> (psnr, ber)
ext = (wm_ext > 128).astype(int)
orig = (wm_orig > 128).astype(int)
ber = np.sum(ext != orig) / ext.size
mse = np.mean((wm_ext.astype(float) - wm_orig.astype(float)) ** 2)
psnr = 10 * math.log10((255 ** 2) / mse)
```

### 4. 鲁棒性测试：`test_robustness`

```python
# 支持翻转、平移、裁剪、对比度调整等攻击
# 返回每种攻击下的提取结果 wm_ext、psnr、ber
```

## 五、脚本运行流程

1. 准备图像：`cover.png` 与 `watermark.png` 放在脚本同级目录。
2. 安装依赖并运行脚本：

   ```bash
   python watermark_dwt.py
   ```
3. 查看 `output/` 目录下生成的水印图、提取图，以及终端打印的 PSNR/BER 指标。

## 六、后续扩展建议

* 添加 JPEG 压缩测试（质量因子 25%、50%、75%）。
* 添加噪声攻击（高斯噪声、椒盐噪声）。
* 幅度旋转、透视变换等几何攻击。
* 支持多级 DWT、多小波类型、彩色图像等。

## 七、总结

该文档提供了一个完整的 DWT 水印系统示例，方便复制并二次开发。欢迎在此基础上根据项目需求进行参数调整和功能扩展。

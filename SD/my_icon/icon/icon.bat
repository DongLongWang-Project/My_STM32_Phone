@echo off
if not exist output mkdir output

for %%f in (*.png) do (
    echo 正在强制铺白底转换: %%f
    
    :: 逻辑：使用 blend 滤镜将原图叠加在纯白背景上
    :: 1. [0:v] 是原图
    :: 2. scale2ref 创建一个和原图一样大的纯白画布
    :: 3. overlay 将原图盖在白画布上，最后转 rgb565le
    ffmpeg -i "%%f" -f lavfi -i "color=c=white:s=1x1" -filter_complex "[1:v][0:v]scale2ref[bg][img];[bg][img]overlay=format=auto,format=rgb565le" -f rawvideo "output/%%~nf.bin"
)

echo.
echo ========================================
echo 转换完成！如果背景还是黑的，请看下面的检查建议。
echo ========================================
pause
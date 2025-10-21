#!/bin/bash
echo "=== Cross Compiling for AIO-3588L ==="

# 交叉编译
aarch64-linux-gnu-gcc ../src/hello.c -o hello_arm -static

# 本地编译（用于对比）
gcc ../src/hello.c -o hello_x86

echo "Compilation completed!"
echo "Files generated:"
echo "  hello_arm  - For AIO-3588L (ARM64)"
echo "  hello_x86  - For PC (x86_64)"

# 查看文件信息
file hello_arm
file hello_x86

# 显示文件大小
echo "File sizes:"
ls -lh hello_*
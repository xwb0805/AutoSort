#!/bin/bash

# AutoSort Linux 编译脚本
# 支持 Ubuntu/Debian, Fedora, Arch Linux

set -e

echo "========================================="
echo "AutoSort Linux 编译脚本"
echo "========================================="

# 检测操作系统
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    echo "无法检测操作系统"
    exit 1
fi

echo "检测到操作系统: $OS"

# 安装依赖
echo ""
echo "正在安装依赖..."

case $OS in
    ubuntu|debian)
        sudo apt update
        sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools qt6-svg-dev
        ;;
    fedora)
        sudo dnf install -y gcc-c++ cmake qt6-qtbase-devel qt6-qtsvg-devel
        ;;
    arch|manjaro)
        sudo pacman -S --noconfirm base-devel cmake qt6-base qt6-svg
        ;;
    *)
        echo "不支持的操作系统: $OS"
        echo "请手动安装以下依赖:"
        echo "  - CMake 3.16+"
        echo "  - Qt 6.x 开发库 (qt6-base-dev, qt6-svg-dev)"
        echo "  - C++17 编译器 (GCC 7+ 或 Clang 5+)"
        exit 1
        ;;
esac

echo "依赖安装完成"

# 创建构建目录
echo ""
echo "正在创建构建目录..."
mkdir -p build
cd build

# 配置项目
echo ""
echo "正在配置项目..."
cmake ..

# 编译
echo ""
echo "正在编译..."
make -j$(nproc)

# 复制资源文件
echo ""
echo "正在复制资源文件..."
cp ../rules.json .
cp ../resources/icon.svg .

echo ""
echo "========================================="
echo "编译完成！"
echo "========================================="
echo ""
echo "可执行文件位置: $(pwd)/AutoSort"
echo ""
echo "运行程序:"
echo "  cd $(pwd)"
echo "  ./AutoSort"
echo ""
echo "或使用命令行参数:"
echo "  ./AutoSort --hidden    # 隐藏启动"
echo "  ./AutoSort --organize  # 立即整理一次"
echo "  ./AutoSort --help      # 显示帮助"
echo ""
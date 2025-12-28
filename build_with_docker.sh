#!/bin/bash

# 使用 Docker 编译 Linux 版本

echo "========================================="
echo "AutoSort Docker 编译脚本"
echo "========================================="

# 检查 Docker 是否安装
if ! command -v docker &> /dev/null; then
    echo "错误: 未找到 Docker"
    echo "请先安装 Docker: https://docs.docker.com/get-docker/"
    exit 1
fi

# 创建输出目录
mkdir -p output

echo "正在构建 Docker 镜像..."
docker build -t autosort-builder .

echo ""
echo "正在编译..."
docker run --rm -v "$(pwd)/output:/output" autosort-builder

echo ""
echo "========================================="
echo "编译完成！"
echo "========================================="
echo ""
echo "输出文件位于: output/"
echo "  - AutoSort (可执行文件)"
echo "  - rules.json (规则配置)"
echo "  - icon.svg (图标)"
echo ""
echo "运行程序:"
echo "  cd output"
echo "  ./AutoSort"
echo ""
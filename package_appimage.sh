#!/bin/bash

# AutoSort AppImage 打包脚本
# 用于创建独立的 Linux 可执行文件

set -e

echo "========================================="
echo "AutoSort AppImage 打包脚本"
echo "========================================="

# 检查是否已编译
if [ ! -f "build/AutoSort" ]; then
    echo "错误: 未找到编译好的 AutoSort"
    echo "请先运行 ./build_linux.sh 进行编译"
    exit 1
fi

# 设置版本
VERSION="1.1.0"
APPDIR="AutoSort.AppDir"

echo "正在创建 AppDir..."

# 创建 AppDir 结构
rm -rf $APPDIR
mkdir -p $APPDIR/usr/bin
mkdir -p $APPDIR/usr/share/applications
mkdir -p $APPDIR/usr/share/icons/hicolor/256x256/apps
mkdir -p $APPDIR/usr/share/metainfo

# 复制可执行文件
cp build/AutoSort $APPDIR/usr/bin/
chmod +x $APPDIR/usr/bin/AutoSort

# 复制资源文件
cp rules.json $APPDIR/usr/bin/
cp resources/icon.svg $APPDIR/usr/share/icons/hicolor/256x256/apps/autosort.svg

# 创建 .desktop 文件
cat > $APPDIR/usr/share/applications/autosort.desktop << EOF
[Desktop Entry]
Name=AutoSort
Comment=自动整理下载文件夹
Exec=AutoSort
Icon=autosort
Type=Application
Categories=Utility;FileManager;
Terminal=false
StartupNotify=true
EOF

# 创建 AppRun 脚本
cat > $APPDIR/AppRun << 'EOF'
#!/bin/bash
SELF=$(readlink -f "$0")
HERE=${SELF%/*}
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${HERE}/usr/plugins:${QT_PLUGIN_PATH}"

cd "${HERE}/usr/bin"
exec "${HERE}/usr/bin/AutoSort" "$@"
EOF
chmod +x $APPDIR/AppRun

echo "正在下载 linuxdeploy..."
if [ ! -f "linuxdeploy-x86_64.AppImage" ]; then
    wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f "linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
fi

echo "正在创建 AppImage..."
./linuxdeploy-x86_64.AppImage \
    --appdir $APPDIR \
    --plugin qt \
    --output appimage

echo ""
echo "========================================="
echo "AppImage 创建完成！"
echo "========================================="
echo ""
echo "输出文件: AutoSort-${VERSION}-x86_64.AppImage"
echo ""
echo "运行程序:"
echo "  ./AutoSort-${VERSION}-x86_64.AppImage"
echo ""
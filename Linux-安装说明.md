# Linux 安装说明

## 方法一：从源代码编译

### 环境要求
- CMake 3.16.0 或更高版本
- Qt 6.x 开发库 (qt6-base-dev, qt6-base-dev-tools)
- C++17 兼容编译器 (GCC 7+, Clang 5+)

### 安装依赖

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-base-dev-tools
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cmake qt6-base
```

### 编译步骤

```bash
# 克隆仓库
git clone https://github.com/xwb0805/AutoSort.git
cd AutoSort

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
make -j$(nproc)

# 安装（可选）
sudo make install
```

### 运行

```bash
# 在 build 目录中运行
./AutoSort
```

## 方法二：使用 AppImage（推荐）

下载 AppImage 文件后：

```bash
# 赋予执行权限
chmod +x AutoSort-x86_64.AppImage

# 运行
./AutoSort-x86_64.AppImage
```

## 方法三：使用 Snap

```bash
# 安装
sudo snap install autosort

# 运行
autosort
```

## 命令行参数

```bash
AutoSort                 # 正常启动
AutoSort --hidden        # 隐藏启动（最小化到托盘）
AutoSort --organize      # 立即整理一次后退出
AutoSort --help          # 显示帮助信息
```

## 系统托盘支持

Linux 系统需要安装系统托盘支持：

**Ubuntu/Debian:**
```bash
sudo apt install libappindicator3-1
```

**Fedora:**
```bash
sudo dnf install libappindicator-gtk3
```

## 开机自启

### 使用 systemd

创建服务文件 `/etc/systemd/system/autosort.service`：

```ini
[Unit]
Description=AutoSort File Organizer
After=network.target

[Service]
Type=simple
User=your_username
WorkingDirectory=/path/to/AutoSort
ExecStart=/path/to/AutoSort/AutoSort --hidden
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

启用服务：
```bash
sudo systemctl daemon-reload
sudo systemctl enable autosort
sudo systemctl start autosort
```

### 使用自动启动应用

在桌面环境中，将 AutoSort 添加到"启动应用程序"中。

## 故障排除

### 缺少 Qt 库

如果运行时提示缺少 Qt 库，安装完整的 Qt6 开发包：

```bash
sudo apt install qt6-base-dev qt6-declarative-dev qt6-svg-dev
```

### 托盘图标不显示

确保安装了 libappindicator：

```bash
sudo apt install libappindicator3-1
```

### 权限问题

确保程序有权限访问下载文件夹：

```bash
# 添加用户到必要的组（如果需要）
sudo usermod -aG your_group your_username
```

## 卸载

### 从源代码编译的版本

```bash
cd build
sudo make uninstall
```

### 删除源代码

```bash
cd ..
rm -rf AutoSort
```

## 获取帮助

- GitHub: https://github.com/xwb0805/AutoSort
- Issues: https://github.com/xwb0805/AutoSort/issues
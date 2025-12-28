# AutoSort - 自动文件整理工具

## 项目概述

AutoSort 是一个基于 Qt6 的跨平台桌面应用程序，用于自动监控和整理下载文件夹中的文件。程序通过系统托盘运行，根据文件扩展名自动将文件分类到不同的文件夹中。

### 主要特性
- **后台监控**: 实时监控下载文件夹，自动整理新文件
- **智能分类**: 根据文件扩展名自动分类（文档、图片、视频、音频等）
- **系统托盘集成**: 最小化到系统托盘，不占用任务栏空间
- **自定义规则**: 支持用户自定义文件分类规则
- **下载完成检测**: 智能检测文件是否下载完成，避免移动未完成的文件
- **命令行支持**: 支持多种启动参数（隐藏启动、立即整理等）

## 技术栈

- **框架**: Qt 6.x (Core, Widgets)
- **语言**: C++17
- **构建系统**: CMake 3.16.0+
- **平台**: 跨平台（Windows, Linux, macOS）

## 项目结构

```
autosort/
├── CMakeLists.txt              # CMake 构建配置
├── README.md                   # 项目说明文档
├── IFLOW.md                    # 开发指南（本文件）
├── rules.json                  # 默认分类规则配置
├── test_compile.cpp           # 测试编译文件
├── build/                     # 构建输出目录
├── resources/                 # 资源文件
│   └── icon.svg              # 应用程序图标
└── src/                      # 源代码目录
    ├── main.cpp              # 程序入口
    ├── mainwindow.cpp/h      # 主窗口类
    ├── fileorganizer.cpp/h   # 文件整理核心逻辑
    └── settingsdialog.cpp/h  # 设置对话框
```

## 核心组件

### 1. MainWindow (主窗口类)
- **文件**: `src/mainwindow.cpp`, `src/mainwindow.h`
- **职责**: 管理主窗口UI、系统托盘图标、用户交互
- **关键功能**:
  - 系统托盘图标和右键菜单
  - 监控开始/停止控制
  - 手动触发文件整理
  - 设置对话框调用

### 2. FileOrganizer (文件整理核心)
- **文件**: `src/fileorganizer.cpp`, `src/fileorganizer.h`
- **职责**: 核心文件整理逻辑和文件夹监控
- **关键功能**:
  - QFileSystemWatcher 实时监控文件夹
  - 智能检测文件是否下载完成
  - 根据扩展名分类文件
  - 规则管理（加载、保存、导入、导出）

### 3. SettingsDialog (设置对话框)
- **文件**: `src/settingsdialog.cpp`, `src/settingsdialog.h`
- **职责**: 应用程序设置管理
- **关键功能**:
  - 下载路径配置
  - 自定义规则编辑
  - 高级选项（日志、备份）
  - 规则导入/导出

### 4. 规则系统
- **配置文件**: `rules.json`
- **默认规则**: 包含 40+ 种文件类型的分类规则
- **分类类别**: 文档、图片、视频、音频、压缩文件、程序、代码等
- **扩展性**: 支持用户添加自定义规则

## 构建和运行

### 环境要求
- CMake 3.16.0 或更高版本
- Qt 6.x 开发库
- C++17 兼容编译器（MinGW, MSVC, GCC, Clang）

### 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目（Windows MinGW）
cmake .. -G "MinGW Makefiles"

# 构建项目
cmake --build .

# 运行程序
./AutoSort.exe
```

### 命令行参数

```bash
AutoSort.exe              # 正常启动
AutoSort.exe --hidden     # 隐藏启动（最小化到托盘）
AutoSort.exe --organize   # 立即整理一次后退出
AutoSort.exe --help       # 显示帮助信息
```

## 开发约定

### 代码风格
- **命名规范**:
  - 类名: PascalCase (例如: `MainWindow`, `FileOrganizer`)
  - 函数名: camelCase (例如: `startMonitoring()`, `organizeFiles()`)
  - 成员变量: m_前缀 + camelCase (例如: `m_fileOrganizer`, `m_downloadPath`)
  - 常量: k_前缀 + PascalCase (如有)

### 文件组织
- 每个类对应一对 `.cpp` 和 `.h` 文件
- 头文件使用 include guards (`#ifndef CLASSNAME_H`)
- Qt 前向声明在 `QT_BEGIN_NAMESPACE` 块中

### 错误处理
- 使用 Qt 的信号/槽机制处理异步操作
- 文件操作前检查路径有效性
- 用户操作提供适当的反馈（消息框、状态栏）

### 配置管理
- 使用 `QSettings` 存储用户配置
- 规则配置使用 JSON 格式
- 默认规则内置于 `rules.json` 文件

## 关键功能实现

### 1. 单实例运行
使用 `QSharedMemory` 确保只有一个程序实例运行：
```cpp
QSharedMemory sharedMemory("AutoSort_SingleInstance");
if (!sharedMemory.create(1)) {
    // 程序已经在运行
}
```

### 2. 系统托盘集成
- 检查系统托盘可用性
- 创建右键菜单
- 处理托盘图标激活事件（单击、双击）

### 3. 文件监控
```cpp
m_watcher = new QFileSystemWatcher(this);
m_watcher->addPath(m_downloadPath);
connect(m_watcher, &QFileSystemWatcher::directoryChanged,
        this, &FileOrganizer::onDirectoryChanged);
```

### 4. 下载完成检测
- **临时文件排除**: 过滤 `.tmp`, `.crdownload`, `.part` 等扩展名
- **文件锁定检测**: 尝试打开文件检测是否被占用
- **延迟处理**: 使用 QTimer 延迟整理，确保下载完成

### 5. 规则系统
- 默认规则从 `rules.json` 加载
- 支持运行时动态添加/删除规则
- 规则持久化到用户配置

## 测试

### 编译测试
```bash
# 测试编译
g++ -std=c++17 test_compile.cpp -o test_compile

# 运行测试
./test_compile
```

### 功能测试
1. **启动测试**: 验证程序能正常启动并显示托盘图标
2. **监控测试**: 创建测试文件，验证自动整理功能
3. **规则测试**: 添加自定义规则，验证规则生效
4. **命令行测试**: 测试各种命令行参数
5. **设置测试**: 验证设置保存和加载

## 部署

### Windows
- 使用 `windeployqt` 部署 Qt 依赖
- 创建安装程序（Inno Setup, NSIS）
- 添加开机启动注册表项

### Linux
- 创建 .desktop 文件
- 打包为 deb/rpm 包
- 添加到系统托盘自动启动

### macOS
- 创建 .app  bundle
- 使用 `macdeployqt` 部署
- 签名和公证

## 常见问题

### 1. 系统托盘不可用
- 检查桌面环境是否支持系统托盘
- Linux 需要安装 `libappindicator` 或 `libayatana-appindicator`

### 2. 文件无法移动
- 检查文件权限
- 确认文件未被其他程序占用
- 验证目标文件夹可写

### 3. 监控不工作
- 检查下载路径是否正确
- 确认文件夹存在
- 查看文件系统是否支持监控（某些网络文件系统可能不支持）

## 待办事项

- [ ] 添加单元测试
- [ ] 支持多语言
- [ ] 添加文件冲突处理策略（覆盖、跳过、重命名）
- [ ] 支持正则表达式规则
- [ ] 添加文件预览功能
- [ ] 支持网络驱动器监控
- [ ] 添加插件系统

## 许可证

本项目仅供学习和个人使用。

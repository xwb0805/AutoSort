# AutoSort - 自动文件整理工具

## 项目概述

AutoSort 是一个基于 Qt6 的跨平台桌面应用程序，用于自动监控和整理下载文件夹中的文件。程序通过系统托盘运行，根据文件扩展名自动将文件分类到不同的文件夹中。

### 主要特性
- **后台监控**: 实时监控下载文件夹，自动整理新文件
- **智能分类**: 根据文件扩展名自动分类（文档、图片、视频、音频等）
- **系统托盘集成**: 最小化到系统托盘，不占用任务栏空间
- **自定义规则**: 支持用户自定义文件分类规则
- **排除扩展名**: 支持排除特定扩展名的文件不进行整理
- **下载完成检测**: 智能检测文件是否下载完成，避免移动未完成的文件
- **命令行支持**: 支持多种启动参数（隐藏启动、立即整理等）
- **Docker 支持**: 支持 Docker 跨平台编译
- **AppImage 打包**: 支持 Linux AppImage 独立打包
- **多路径监控**: 支持同时监控多个文件夹
- **正则表达式规则**: 支持使用正则表达式匹配文件名
- **冲突处理策略**: 提供多种文件冲突处理选项（覆盖、跳过、重命名等）
- **统计功能**: 记录整理文件数量、节省空间等统计信息
- **日志系统**: 完整的日志记录和查看功能
- **文件预览**: 支持图片和文本文件的预览
- **文件搜索**: 强大的文件搜索功能
- **批量重命名**: 支持批量重命名文件
- **文件过滤**: 高级文件过滤功能
- **多语言支持**: 支持中英文界面切换

## 技术栈

- **框架**: Qt 6.x (Core, Widgets)
- **语言**: C++17
- **构建系统**: CMake 3.16.0+
- **平台**: 跨平台（Windows, Linux, macOS）
- **容器化**: Docker（可选）
- **国际化**: Qt Linguist (TS 文件)

## 项目结构

```
autosort/
├── CMakeLists.txt                   # CMake 构建配置
├── README.md                        # 项目说明文档
├── IFLOW.md                         # 开发指南（本文件）
├── rules.json                       # 默认分类规则配置（100+ 文件类型）
├── test_compile.cpp                 # 测试编译文件
├── build_linux.sh                   # Linux 自动编译脚本
├── build_with_docker.sh             # Docker 编译脚本
├── package_appimage.sh              # AppImage 打包脚本
├── Dockerfile                       # Docker 构建配置
├── Linux-安装说明.md                # Linux 安装详细说明
├── build/                           # 构建输出目录
├── release/                         # 发布输出目录
├── singlefile/                      # 单文件打包输出目录
├── resources/                       # 资源文件
│   ├── icon.svg                     # 应用程序图标
│   └── translations/                # 翻译文件
│       └── autosort_en.ts           # 英文翻译
├── android_autosort/                # Android 版本
│   ├── app/                         # Android 应用源码
│   ├── build.gradle                 # Gradle 构建配置
│   └── IFLOW.md                     # Android 版本开发指南
└── src/                             # 源代码目录
    ├── main.cpp                     # 程序入口
    ├── common.h                     # 公共定义（枚举、结构体）
    ├── mainwindow.cpp/h             # 主窗口类
    ├── fileorganizer.cpp/h          # 文件整理核心逻辑
    ├── settingsdialog.cpp/h         # 设置对话框
    ├── statistics.cpp/h             # 统计功能
    ├── logmanager.cpp/h             # 日志管理
    ├── filepreviewdialog.cpp/h      # 文件预览对话框
    ├── logviewerdialog.cpp/h        # 日志查看器对话框
    ├── statisticsdialog.cpp/h       # 统计信息对话框
    ├── conflictresolutiondialog.cpp/h # 冲突解决对话框
    ├── filesearchdialog.cpp/h       # 文件搜索对话框
    ├── batchrenamedialog.cpp/h      # 批量重命名对话框
    └── filefilterdialog.cpp/h       # 文件过滤对话框
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
  - 整理完成通知
  - 最小化到托盘处理
  - 统计信息查看
  - 日志查看器
  - 文件搜索
  - 批量重命名
  - 文件过滤

### 2. FileOrganizer (文件整理核心)
- **文件**: `src/fileorganizer.cpp`, `src/fileorganizer.h`
- **职责**: 核心文件整理逻辑和文件夹监控
- **关键功能**:
  - QFileSystemWatcher 实时监控文件夹
  - 智能检测文件是否下载完成
  - 根据扩展名分类文件
  - 规则管理（加载、保存、导入、导出）
  - 排除扩展名管理
  - 文件夹移动支持
  - 默认规则初始化（100+ 文件类型）
  - **多路径监控**: 支持同时监控多个文件夹
  - **正则表达式规则**: 支持使用正则表达式匹配文件名
  - **冲突处理策略**: 提供多种文件冲突处理选项

### 3. SettingsDialog (设置对话框)
- **文件**: `src/settingsdialog.cpp`, `src/settingsdialog.h`
- **职责**: 应用程序设置管理
- **关键功能**:
  - 下载路径配置
  - 自定义规则编辑（添加、删除、导入、导出）
  - 排除扩展名管理
  - 高级选项（开机自启、最小化到托盘、启动时最小化）
  - 整理延迟设置
  - 通知开关
  - 冲突处理策略设置

### 4. Statistics (统计功能)
- **文件**: `src/statistics.cpp`, `src/statistics.h`
- **职责**: 记录和展示文件整理统计信息
- **关键功能**:
  - 记录整理文件数量
  - 计算节省的磁盘空间
  - 按时间统计（今日、本周、本月）
  - 按文件类型统计
  - 按目标文件夹统计
  - 统计数据持久化

### 5. LogManager (日志管理)
- **文件**: `src/logmanager.cpp`, `src/logmanager.h`
- **职责**: 管理应用程序日志
- **关键功能**:
  - 多级别日志（Info、Success、Warning、Error）
  - 日志持久化
  - 按类型筛选日志
  - 按日期范围筛选日志
  - 日志导出功能

### 6. FilePreviewDialog (文件预览对话框)
- **文件**: `src/filepreviewdialog.cpp`, `src/filepreviewdialog.h`
- **职责**: 提供文件预览功能
- **关键功能**:
  - 图片预览（JPG、PNG、GIF、BMP、WEBP）
  - 文本预览（TXT、MD、JSON、XML、HTML、CSS、JS、CPP、H、PY、JAVA）
  - 文件详细信息展示
  - 文件属性查看

### 7. ConflictResolutionDialog (冲突解决对话框)
- **文件**: `src/conflictresolutiondialog.cpp`, `src/conflictresolutiondialog.h`
- **职责**: 处理文件冲突
- **关键功能**:
  - 显示源文件和目标文件信息
  - 提供多种冲突处理选项
  - 支持重命名文件
  - 支持"应用到所有"选项

### 8. FileSearchDialog (文件搜索对话框)
- **文件**: `src/filesearchdialog.cpp`, `src/filesearchdialog.h`
- **职责**: 提供文件搜索功能
- **关键功能**:
  - 按名称搜索
  - 按扩展名搜索
  - 按大小搜索
  - 支持子目录搜索
  - 大小写敏感选项
  - 搜索结果展示

### 9. BatchRenameDialog (批量重命名对话框)
- **文件**: `src/batchrenamedialog.cpp`, `src/batchrenamedialog.h`
- **职责**: 提供批量重命名功能
- **关键功能**:
  - 添加前缀/后缀
  - 使用模式替换
  - 序号重命名
  - 查找替换
  - 预览功能

### 10. FileFilterDialog (文件过滤对话框)
- **文件**: `src/filefilterdialog.cpp`, `src/filefilterdialog.h`
- **职责**: 提供高级文件过滤功能
- **关键功能**:
  - 按名称过滤
  - 按扩展名过滤
  - 按大小过滤
  - 按日期过滤
  - 自定义过滤规则

### 11. 规则系统
- **配置文件**: `rules.json`
- **默认规则**: 包含 100+ 种文件类型的分类规则
- **分类类别**:
  - **文档**: PDF, DOC, XLS, PPT, TXT, RTF, ODT, CSV, MD, EPUB, MOBI, AZW, DJVU, CHM 等
  - **图片**: JPG, PNG, GIF, BMP, TIFF, SVG, WEBP, ICO, PSD, AI, EPS, RAW 等
  - **视频**: MP4, AVI, MKV, MOV, WMV, FLV, WEBM, 3GP, MPG, VOB 等
  - **音频**: MP3, WAV, FLAC, AAC, OGG, WMA, M4A, APE, ALAC 等
  - **压缩文件**: ZIP, RAR, 7Z, TAR, GZ, BZ2, XZ, CAB, ISO 等
  - **程序**: EXE, MSI, DEB, RPM, DMG, PKG, APPIMAGE, APK, APP, BAT, SH 等
  - **代码**: CPP, C, H, JAVA, PY, JS, TS, HTML, CSS, PHP, RB, GO, RS, SWIFT, KT, JSON, XML, YAML, SQL 等
  - **字体**: TTF, OTF, WOFF 等
  - **数据库**: DB, SQLITE, MDB, ACCDB 等
- **扩展性**: 支持用户添加自定义规则和排除规则
- **正则表达式**: 支持使用正则表达式匹配文件名进行分类

## 公共定义

### ConflictAction (冲突处理策略)
- `Overwrite`: 覆盖当前文件
- `Skip`: 跳过当前文件
- `Rename`: 重命名当前文件
- `OverwriteAll`: 覆盖所有冲突文件
- `SkipAll`: 跳过所有冲突文件
- `Ask`: 询问用户
- `Cancel`: 取消

### LogType (日志类型)
- `Info`: 信息日志
- `Success`: 成功日志
- `Warning`: 警告日志
- `Error`: 错误日志

### LogEntry (日志条目)
- `timestamp`: 时间戳
- `type`: 日志类型
- `message`: 日志消息

## 构建和运行

### 环境要求
- CMake 3.16.0 或更高版本
- Qt 6.x 开发库
- C++17 兼容编译器（MinGW, MSVC, GCC, Clang）

### Windows 构建

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目（MinGW）
cmake .. -G "MinGW Makefiles"

# 或配置项目（MSVC）
cmake .. -G "Visual Studio 17 2022"

# 构建项目
cmake --build .

# 运行程序
./AutoSort.exe
```

### Linux 构建（自动脚本）

```bash
# 使用自动编译脚本（推荐）
chmod +x build_linux.sh
./build_linux.sh

# 编译完成后在 build 目录运行
cd build
./AutoSort
```

### Linux 构建（手动）

```bash
# 安装依赖
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-base-dev-tools qt6-svg-dev

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
make -j$(nproc)

# 运行程序
./AutoSort
```

### Docker 构建（跨平台）

```bash
# 使用 Docker 编译脚本
chmod +x build_with_docker.sh
./build_with_docker.sh

# 输出文件在 output/ 目录
cd output
./AutoSort
```

### AppImage 打包

```bash
# 先编译项目
./build_linux.sh

# 打包为 AppImage
chmod +x package_appimage.sh
./package_appimage.sh

# 运行 AppImage
./AutoSort-1.1.0-x86_64.AppImage
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
  - 常量: k_前缀 + PascalCase（如有）
  - 枚举: PascalCase (例如: `ConflictAction`, `LogType`)
  - 结构体: PascalCase (例如: `LogEntry`, `StatisticsData`, `FilterRule`)

### 文件组织
- 每个类对应一对 `.cpp` 和 `.h` 文件
- 头文件使用 include guards (`#ifndef CLASSNAME_H`)
- Qt 前向声明在 `QT_BEGIN_NAMESPACE` 块中
- 公共定义放在 `common.h` 中

### 错误处理
- 使用 Qt 的信号/槽机制处理异步操作
- 文件操作前检查路径有效性
- 用户操作提供适当的反馈（消息框、状态栏）
- 文件锁定检测避免移动正在使用的文件
- 使用日志系统记录错误信息

### 配置管理
- 使用 `QSettings` 存储用户配置
- 规则配置使用 JSON 格式
- 默认规则内置于 `rules.json` 文件
- 排除扩展名独立管理
- 正则表达式规则独立管理

### 国际化
- 使用 Qt Linguist 进行翻译
- 翻译文件存放在 `resources/translations/` 目录
- 使用 `tr()` 函数标记需要翻译的字符串
- 当前支持中文和英文

## 关键功能实现

### 1. 单实例运行
使用 `QSharedMemory` 确保只有一个程序实例运行：
```cpp
QSharedMemory sharedMemory("AutoSort_SingleInstance");
if (!sharedMemory.create(1)) {
    QMessageBox::information(nullptr, "自动整理工具", "程序已经在运行中");
    return 0;
}
```

### 2. 系统托盘集成
- 检查系统托盘可用性
- 创建右键菜单
- 处理托盘图标激活事件（单击、双击）
- 最小化到托盘而非关闭

### 3. 文件监控
```cpp
m_watcher = new QFileSystemWatcher(this);
m_watcher->addPath(m_downloadPath);
connect(m_watcher, &QFileSystemWatcher::directoryChanged,
        this, &FileOrganizer::onDirectoryChanged);
connect(m_watcher, &QFileSystemWatcher::fileChanged,
        this, &FileOrganizer::onFileChanged);
```

### 4. 多路径监控
```cpp
void FileOrganizer::addMonitorPath(const QString &path) {
    if (!m_monitorPaths.contains(path)) {
        m_monitorPaths.append(path);
        m_watcher->addPath(path);
    }
}

void FileOrganizer::setMonitorPaths(const QStringList &paths) {
    // 移除旧路径
    for (const QString &path : m_monitorPaths) {
        m_watcher->removePath(path);
    }
    // 添加新路径
    m_monitorPaths = paths;
    for (const QString &path : m_monitorPaths) {
        m_watcher->addPath(path);
    }
}
```

### 5. 正则表达式规则
```cpp
void FileOrganizer::addRegexRule(const QString &pattern, const QString &folder) {
    QRegularExpression regex(pattern);
    if (regex.isValid()) {
        m_regexRules[pattern] = regex;
        // 保存到配置
        saveRegexRules();
    }
}

QString FileOrganizer::matchRegexRule(const QString &fileName) const {
    for (auto it = m_regexRules.constBegin(); it != m_regexRules.constEnd(); ++it) {
        if (it.value().match(fileName).hasMatch()) {
            return it.key();
        }
    }
    return QString();
}
```

### 6. 下载完成检测
- **临时文件排除**: 过滤 `.tmp`, `.crdownload`, `.part` 等扩展名
- **文件锁定检测**: 尝试打开文件检测是否被占用
- **延迟处理**: 使用 QTimer 延迟整理，确保下载完成
- **排除扩展名**: 用户可自定义排除特定扩展名

### 7. 规则系统
- 默认规则从 `rules.json` 加载（100+ 文件类型）
- 支持运行时动态添加/删除规则
- 规则持久化到用户配置
- 支持规则导入/导出
- 支持正则表达式规则

### 8. 文件夹处理
- 支持文件移动
- 支持文件夹移动（整个文件夹）
- 自动创建目标文件夹（如不存在）
- 避免移动分类文件夹本身

### 9. 冲突处理
```cpp
QString FileOrganizer::handleConflict(const QString &sourcePath, const QString &targetPath) {
    switch (m_conflictAction) {
        case ConflictAction::Overwrite:
            // 覆盖文件
            break;
        case ConflictAction::Skip:
            // 跳过文件
            break;
        case ConflictAction::Rename:
            // 重命名文件（添加序号）
            int counter = 1;
            QString newName;
            do {
                QFileInfo info(targetPath);
                newName = info.baseName() + QString(" (%1)").arg(counter) + "." + info.suffix();
                counter++;
            } while (QFile::exists(newName));
            return newName;
        case ConflictAction::Ask:
            // 询问用户
            emit fileConflict(sourcePath, targetPath);
            break;
        default:
            break;
    }
    return QString();
}
```

### 10. 统计功能
```cpp
void Statistics::recordFileOrganized(const QString &extension, const QString &folder, qint64 fileSize) {
    m_data.totalFilesOrganized++;
    m_data.totalSpaceSaved += fileSize;
    m_data.filesByType[extension]++;
    m_data.filesByFolder[folder]++;
    m_data.lastOrganizedTime = QDateTime::currentDateTime();
    updatePeriodicCounts();
    saveStatistics();
    emit statisticsUpdated();
}
```

### 11. 日志系统
```cpp
void LogManager::addLog(LogType type, const QString &message) {
    LogEntry entry(type, message);
    m_logs.append(entry);
    if (m_logs.size() > m_maxLogEntries) {
        m_logs.removeFirst();
    }
    saveLogs();
    emit logAdded(entry);
}
```

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
4. **排除测试**: 添加排除扩展名，验证排除功能
5. **命令行测试**: 测试各种命令行参数
6. **设置测试**: 验证设置保存和加载
7. **文件夹测试**: 验证文件夹移动功能
8. **多路径测试**: 验证多路径监控功能
9. **正则表达式测试**: 验证正则表达式规则匹配
10. **冲突处理测试**: 验证各种冲突处理策略
11. **统计功能测试**: 验证统计信息记录和显示
12. **日志功能测试**: 验证日志记录和查看功能
13. **文件预览测试**: 验证图片和文本预览功能
14. **文件搜索测试**: 验证文件搜索功能
15. **批量重命名测试**: 验证批量重命名功能
16. **文件过滤测试**: 验证文件过滤功能
17. **多语言测试**: 验证中英文切换功能

## 部署

### Windows
- 使用 `windeployqt` 部署 Qt 依赖
- 创建安装程序（Inno Setup, NSIS）
- 添加开机启动注册表项

### Linux
- 创建 .desktop 文件
- 使用 systemd 服务实现开机自启
- 打包为 AppImage（推荐）
- 打包为 deb/rpm 包
- 添加到系统托盘自动启动

### macOS
- 创建 .app bundle
- 使用 `macdeployqt` 部署
- 签名和公证

### Docker
- 使用 Dockerfile 进行容器化
- 支持跨平台编译

## 常见问题

### 1. 系统托盘不可用
- 检查桌面环境是否支持系统托盘
- Linux 需要安装 `libappindicator` 或 `libayatana-appindicator`
- Windows 确保系统托盘区域可见

### 2. 文件无法移动
- 检查文件权限
- 确认文件未被其他程序占用
- 验证目标文件夹可写
- 检查文件是否在排除列表中

### 3. 监控不工作
- 检查下载路径是否正确
- 确认文件夹存在
- 查看文件系统是否支持监控（某些网络文件系统可能不支持）
- 检查监控是否已启动

### 4. Linux 编译失败
- 确保安装了 Qt6 开发库：`qt6-base-dev`, `qt6-base-dev-tools`, `qt6-svg-dev`
- 检查 CMake 版本是否 >= 3.16.0
- 检查编译器是否支持 C++17

### 5. Docker 编译失败
- 确保 Docker 已安装并运行
- 检查网络连接（需要下载依赖）
- 检查输出目录权限

### 6. 正则表达式不匹配
- 检查正则表达式语法是否正确
- 确保正则表达式模式与预期匹配
- 使用测试工具验证正则表达式

### 7. 冲突处理不生效
- 检查冲突处理策略设置
- 确保选择了正确的处理方式
- 查看日志了解冲突处理详情

## 相关项目

### Android 版本
项目包含完整的 Android 版本，位于 `android_autosort/` 目录：
- 使用 Kotlin + Material Design 3
- 前台服务实现后台监控
- 支持自定义规则和排除
- 详见 `android_autosort/IFLOW.md`

## 待办事项

- [ ] 添加单元测试
- [ ] 支持更多语言（i18n）
- [ ] 支持网络驱动器监控
- [ ] 添加插件系统
- [ ] 完成新版设置对话框（settingsdialog_new.cpp）
- [ ] 支持云存储监控（Google Drive, OneDrive, Dropbox）
- [ ] 添加文件内容搜索功能
- [ ] 支持自定义脚本规则
- [ ] 添加文件同步功能
- [ ] 支持文件版本控制

## 许可证

本项目仅供学习和个人使用。

## 获取帮助

- GitHub: https://github.com/xwb0805/AutoSort
- Issues: https://github.com/xwb0805/AutoSort/issues
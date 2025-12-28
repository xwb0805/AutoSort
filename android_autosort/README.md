# AutoSort Android 版本

这是 AutoSort 的 Android 版本，用于自动监控和整理下载文件夹中的文件。

## 功能特性

- **后台监控**: 使用前台服务持续监控下载文件夹
- **智能分类**: 根据文件扩展名自动分类文件
- **自定义规则**: 支持用户添加、编辑、导入和导出分类规则
- **开机自启**: 支持系统启动后自动运行
- **Material Design 3**: 现代化的 Material Design 界面
- **实时日志**: 查看文件整理操作日志

## 技术栈

- **语言**: Kotlin
- **框架**: Android SDK (API 26-34)
- **UI**: Material Design 3 + ViewBinding
- **异步**: Kotlin Coroutines
- **数据持久化**: SharedPreferences + Gson

## 项目结构

```
android_autosort/
├── app/
│   └── src/main/
│       ├── java/com/autosort/app/
│       │   ├── MainActivity.kt          # 主界面
│       │   ├── SettingsActivity.kt      # 设置界面
│       │   ├── model/                   # 数据模型
│       │   │   ├── SortRule.kt
│       │   │   └── LogEntry.kt
│       │   ├── core/                    # 核心逻辑
│       │   │   └── FileOrganizer.kt
│       │   ├── service/                 # 服务
│       │   │   └── FileMonitorService.kt
│       │   ├── receiver/                # 广播接收器
│       │   │   └── BootReceiver.kt
│       │   ├── adapter/                 # RecyclerView 适配器
│       │   │   ├── RulesAdapter.kt
│       │   │   └── LogAdapter.kt
│       │   └── utils/                   # 工具类
│       │       └── PreferencesManager.kt
│       ├── res/                         # 资源文件
│       │   ├── layout/                  # 布局文件
│       │   ├── values/                  # 值资源
│       │   ├── drawable/                # 图标
│       │   └── xml/                     # XML 配置
│       └── AndroidManifest.xml          # 应用清单
├── build.gradle                         # 项目级构建配置
├── settings.gradle                      # 项目设置
└── gradle.properties                    # Gradle 属性
```

## 构建和运行

### 环境要求

- Android Studio Arctic Fox 或更高版本
- JDK 17 或更高版本
- Android SDK API 34
- Gradle 8.0 或更高版本

### 构建步骤

1. 使用 Android Studio 打开项目
2. 等待 Gradle 同步完成
3. 连接 Android 设备或启动模拟器
4. 点击运行按钮或使用命令行：

```bash
# 构建调试版本
./gradlew assembleDebug

# 安装到设备
./gradlew installDebug
```

### 命令行构建

```bash
# 进入项目目录
cd android_autosort

# 构建
gradlew.bat assembleDebug

# 安装
gradlew.bat installDebug
```

## 使用说明

### 首次使用

1. 启动应用后，授予存储权限
2. 在 Android 11+ 设备上，需要授予"管理所有文件"权限
3. 选择要监控的下载文件夹（默认为系统下载目录）

### 基本操作

- **开始/停止监控**: 使用主界面的开关控制
- **立即整理**: 点击"立即整理"按钮手动触发
- **查看规则**: 主界面显示所有分类规则
- **编辑规则**: 进入设置界面添加、编辑、导入或导出规则

### 设置选项

- **下载路径**: 选择要监控的文件夹
- **开机自启**: 系统启动后自动运行服务
- **显示通知**: 是否显示文件整理通知
- **规则管理**: 添加、编辑、导入、导出分类规则
- **日志查看**: 查看文件整理操作日志

## 默认分类规则

应用包含以下默认分类规则：

| 规则名称 | 扩展名 | 目标文件夹 |
|---------|--------|-----------|
| 文档 | pdf, doc, docx, txt, rtf, odt, xls, xlsx, ppt, pptx | Documents |
| 图片 | jpg, jpeg, png, gif, bmp, webp, svg, ico | Images |
| 视频 | mp4, avi, mkv, mov, wmv, flv, webm, m4v | Videos |
| 音频 | mp3, wav, flac, aac, ogg, wma, m4a | Audio |
| 压缩文件 | zip, rar, 7z, tar, gz, bz2 | Archives |
| 程序 | exe, msi, apk, dmg, deb, rpm | Programs |
| 代码 | cpp, h, hpp, java, py, js, ts, html, css, json, xml, yaml | Code |

## 权限说明

应用需要以下权限：

- `READ_EXTERNAL_STORAGE`: 读取外部存储
- `WRITE_EXTERNAL_STORAGE`: 写入外部存储
- `MANAGE_EXTERNAL_STORAGE`: 管理所有文件（Android 11+）
- `FOREGROUND_SERVICE`: 前台服务
- `POST_NOTIFICATIONS`: 发送通知（Android 13+）
- `RECEIVE_BOOT_COMPLETED`: 开机自启

## 注意事项

1. **存储权限**: Android 11+ 需要授予"管理所有文件"权限才能访问所有文件夹
2. **后台限制**: Android 系统可能会限制后台服务，建议将应用加入电池优化白名单
3. **文件监控**: 使用定期检查而非实时监控，以节省电量
4. **下载检测**: 应用会自动跳过临时文件（.tmp, .crdownload 等）和未完成的下载

## 与桌面版的区别

1. **监控方式**: Android 版使用定期检查而非文件系统监听
2. **界面设计**: 使用 Material Design 3 适配移动设备
3. **权限管理**: 需要处理 Android 的存储权限系统
4. **后台运行**: 使用前台服务确保服务不被系统杀死

## 许可证

本项目仅供学习和个人使用。
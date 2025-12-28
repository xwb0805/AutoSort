# AutoSort Android 版本 - 开发指南

## 项目概述

AutoSort Android 版本是一个基于 Kotlin 的 Android 应用程序，用于自动监控和整理下载文件夹中的文件。程序通过前台服务在后台运行，根据文件扩展名自动将文件分类到不同的文件夹中。

### 主要特性
- **后台监控**: 使用前台服务持续监控下载文件夹（每30秒检查一次）
- **智能分类**: 根据文件扩展名自动分类（文档、图片、视频、音频等）
- **自定义规则**: 支持用户自定义文件分类规则（添加、编辑、导入、导出）
- **开机自启**: 支持系统启动后自动运行
- **Material Design 3**: 现代化的 Material Design 界面
- **实时日志**: 查看文件整理操作日志
- **下载完成检测**: 智能检测文件是否下载完成，避免移动未完成的文件

## 技术栈

- **语言**: Kotlin
- **框架**: Android SDK (API 26-34)
- **UI**: Material Design 3 + ViewBinding
- **异步**: Kotlin Coroutines
- **数据持久化**: SharedPreferences + Gson
- **构建系统**: Gradle 8.0+
- **最低 SDK**: API 26 (Android 8.0)
- **目标 SDK**: API 34 (Android 14)

## 项目结构

```
android_autosort/
├── app/
│   └── src/main/
│       ├── java/com/autosort/app/
│       │   ├── MainActivity.kt          # 主界面，处理权限请求、监控控制
│       │   ├── SettingsActivity.kt      # 设置界面，规则管理
│       │   ├── model/                   # 数据模型
│       │   │   ├── SortRule.kt          # 分类规则数据模型
│       │   │   └── LogEntry.kt          # 日志条目数据模型
│       │   ├── core/                    # 核心逻辑
│       │   │   └── FileOrganizer.kt     # 文件整理核心逻辑
│       │   ├── service/                 # 服务
│       │   │   └── FileMonitorService.kt # 前台服务，定期检查并整理文件
│       │   ├── receiver/                # 广播接收器
│       │   │   └── BootReceiver.kt      # 开机自启动广播接收器
│       │   ├── adapter/                 # RecyclerView 适配器
│       │   │   ├── RulesAdapter.kt      # 规则列表适配器
│       │   │   └── LogAdapter.kt        # 日志列表适配器
│       │   └── utils/                   # 工具类
│       │       └── PreferencesManager.kt # 偏好设置管理
│       ├── res/                         # 资源文件
│       │   ├── layout/                  # 布局文件
│       │   │   ├── activity_main.xml    # 主界面布局
│       │   │   ├── activity_settings.xml # 设置界面布局
│       │   │   ├── dialog_edit_rule.xml # 编辑规则对话框布局
│       │   │   ├── item_log.xml         # 日志条目布局
│       │   │   └── item_rule.xml        # 规则条目布局
│       │   ├── values/                  # 值资源
│       │   │   ├── colors.xml           # 颜色定义
│       │   │   ├── strings.xml          # 字符串资源
│       │   │   └── themes.xml           # 主题定义
│       │   ├── drawable/                # 图标资源
│       │   │   ├── ic_edit.xml          # 编辑图标
│       │   │   ├── ic_settings.xml      # 设置图标
│       │   │   ├── ic_sort.xml          # 整理图标
│       │   │   ├── status_indicator_off.xml # 状态指示器（关闭）
│       │   │   └── status_indicator_on.xml  # 状态指示器（开启）
│       │   └── xml/                     # XML 配置
│       │       ├── backup_rules.xml     # 备份规则
│       │       ├── data_extraction_rules.xml # 数据提取规则
│       │       └── file_paths.xml       # 文件访问路径
│       └── AndroidManifest.xml          # 应用清单
├── build.gradle                         # 项目级构建配置
├── settings.gradle                      # 项目设置
├── gradle.properties                    # Gradle 属性
└── local.properties                     # 本地配置（SDK 路径等）
```

## 核心组件

### 1. MainActivity (主界面类)
- **文件**: `app/src/main/java/com/autosort/app/MainActivity.kt`
- **职责**: 管理主界面UI、权限请求、监控控制
- **关键功能**:
  - 请求存储权限（包括 Android 11+ 的 MANAGE_EXTERNAL_STORAGE）
  - 启动/停止文件监控服务
  - 手动触发文件整理
  - 显示当前状态和规则列表
  - 跳转到设置界面

### 2. FileOrganizer (文件整理核心)
- **文件**: `app/src/main/java/com/autosort/app/core/FileOrganizer.kt`
- **职责**: 核心文件整理逻辑
- **关键功能**:
  - 遍历源文件夹中的文件
  - 根据扩展名匹配分类规则
  - 创建目标文件夹（如不存在）
  - 移动文件到对应文件夹
  - 记录操作日志
  - 临时文件过滤（.tmp, .crdownload, .part 等）
  - 下载完成检测

### 3. FileMonitorService (文件监控服务)
- **文件**: `app/src/main/java/com/autosort/app/service/FileMonitorService.kt`
- **职责**: 前台服务，定期检查并整理文件
- **关键功能**:
  - 创建前台服务通知
  - 每30秒检查一次文件
  - 支持一次性整理模式
  - 显示整理通知（可选）
  - 使用 Kotlin Coroutines 进行异步操作

### 4. SettingsActivity (设置界面)
- **文件**: `app/src/main/java/com/autosort/app/SettingsActivity.kt`
- **职责**: 应用程序设置管理
- **关键功能**:
  - 下载路径配置
  - 自定义规则编辑（添加、编辑、删除）
  - 规则导入/导出
  - 高级选项（开机自启、通知开关）
  - 日志查看

### 5. PreferencesManager (偏好设置管理)
- **文件**: `app/src/main/java/com/autosort/app/utils/PreferencesManager.kt`
- **职责**: 管理应用设置和规则数据
- **关键功能**:
  - 下载路径存储
  - 监控状态存储
  - 规则列表序列化/反序列化（使用 Gson）
  - 默认规则定义
  - 开机自启设置
  - 通知开关设置

### 6. BootReceiver (开机自启动接收器)
- **文件**: `app/src/main/java/com/autosort/app/receiver/BootReceiver.kt`
- **职责**: 监听系统启动事件
- **关键功能**:
  - 监听 BOOT_COMPLETED 广播
  - 检查是否启用开机自启
  - 启动文件监控服务

### 7. 数据模型

#### SortRule (分类规则)
- **文件**: `app/src/main/java/com/autosort/app/model/SortRule.kt`
- **字段**:
  - `id`: 规则唯一标识符
  - `name`: 规则名称
  - `extensions`: 文件扩展名列表
  - `targetFolder`: 目标文件夹名称
  - `enabled`: 是否启用

#### LogEntry (日志条目)
- **文件**: `app/src/main/java/com/autosort/app/model/LogEntry.kt`
- **字段**:
  - `timestamp`: 时间戳
  - `message`: 日志消息
  - `type`: 日志类型（SUCCESS, ERROR, INFO）

## 构建和运行

### 环境要求
- Android Studio Arctic Fox 或更高版本
- JDK 17 或更高版本
- Android SDK API 34
- Gradle 8.0 或更高版本
- Kotlin 1.9.0 或更高版本

### 构建步骤

#### 使用 Android Studio
1. 使用 Android Studio 打开项目
2. 等待 Gradle 同步完成
3. 连接 Android 设备或启动模拟器
4. 点击运行按钮

#### 使用命令行

```bash
# 进入项目目录
cd android_autosort

# Windows
gradlew.bat assembleDebug

# Linux/macOS
./gradlew assembleDebug

# 安装到设备
gradlew.bat installDebug
```

### 清理构建

```bash
# 清理构建产物
gradlew.bat clean

# 重新构建
gradlew.bat assembleDebug
```

### 运行测试

```bash
# 运行单元测试
gradlew.bat test

# 运行仪器测试
gradlew.bat connectedAndroidTest
```

## 开发约定

### 代码风格
- **命名规范**:
  - 类名: PascalCase (例如: `MainActivity`, `FileOrganizer`)
  - 函数名: camelCase (例如: `startMonitoring()`, `organizeFiles()`)
  - 变量名: camelCase
  - 常量: UPPER_SNAKE_CASE

- **Kotlin 惯用法**:
  - 使用数据类（data class）表示数据模型
  - 使用 val 优先于 var
  - 使用扩展函数简化代码
  - 使用协程处理异步操作

### 文件组织
- 每个类对应一个 `.kt` 文件
- 使用包名组织代码（com.autosort.app.*）
- 按功能模块分组（model, core, service, adapter, utils）

### ViewBinding 使用
- 在 Activity 中使用 ViewBinding 绑定布局
- 在 `onCreate` 中初始化 binding
- 在 `onDestroy` 中清理 binding（如需要）

### 权限处理
- 使用 `ActivityResultContracts` 请求权限
- Android 11+ 需要请求 `MANAGE_EXTERNAL_STORAGE` 权限
- Android 13+ 需要请求 `POST_NOTIFICATIONS` 权限

### 异步操作
- 使用 Kotlin Coroutines 处理异步操作
- 文件操作在 `Dispatchers.IO` 中执行
- 使用 `withContext` 切换上下文

### 错误处理
- 使用 try-catch 处理文件操作异常
- 向用户显示友好的错误提示
- 记录错误日志

## 关键功能实现

### 1. 前台服务
```kotlin
override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
    startForeground(NOTIFICATION_ID, createNotification())
    // ...
    return START_STICKY
}
```

### 2. 权限请求
```kotlin
private val requestPermissionLauncher = registerForActivityResult(
    ActivityResultContracts.RequestMultiplePermissions()
) { permissions ->
    // 处理权限结果
}
```

### 3. 数据持久化
```kotlin
// 保存规则
fun saveRules(rules: List<SortRule>) {
    val json = gson.toJson(rules)
    prefs.edit().putString(KEY_RULES, json).apply()
}

// 加载规则
fun getRules(): List<SortRule> {
    val json = prefs.getString(KEY_RULES, null)
    return if (json != null) {
        gson.fromJson(json, object : TypeToken<List<SortRule>>() {}.type)
    } else {
        getDefaultRules()
    }
}
```

### 4. 定时检查
```kotlin
monitorJob = serviceScope.launch {
    while (isActive) {
        if (prefs.isMonitoringEnabled) {
            organizeFiles()
        }
        delay(30000) // 每30秒检查一次
    }
}
```

### 5. 文件移动
```kotlin
val targetFile = File(targetFolder, file.name)
if (file.renameTo(targetFile)) {
    // 移动成功
} else {
    // 移动失败
}
```

## 依赖库

### 核心依赖
- `androidx.core:core-ktx:1.12.0` - Android KTX 扩展
- `androidx.appcompat:appcompat:1.6.1` - AppCompat 库
- `com.google.android.material:material:1.11.0` - Material Design 组件
- `androidx.constraintlayout:constraintlayout:2.1.4` - 约束布局
- `androidx.work:work-runtime-ktx:2.9.0` - WorkManager（预留）
- `androidx.preference:preference-ktx:1.2.1` - 偏好设置
- `org.jetbrains.kotlinx:kotlinx-coroutines-android:1.7.3` - Kotlin 协程
- `com.google.code.gson:gson:2.10.1` - JSON 序列化

### 测试依赖
- `junit:junit:4.13.2` - 单元测试
- `androidx.test.ext:junit:1.1.5` - Android 测试扩展
- `androidx.test.espresso:espresso-core:3.5.1` - UI 测试

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
- `FOREGROUND_SERVICE_DATA_SYNC`: 数据同步类型前台服务
- `POST_NOTIFICATIONS`: 发送通知（Android 13+）
- `RECEIVE_BOOT_COMPLETED`: 开机自启

## 注意事项

### 1. 存储权限
- Android 11+ 需要授予"管理所有文件"权限才能访问所有文件夹
- 使用 `Environment.isExternalStorageManager()` 检查权限状态
- 引导用户到系统设置页面授予权限

### 2. 后台限制
- Android 系统可能会限制后台服务
- 建议将应用加入电池优化白名单
- 使用前台服务确保服务不被系统杀死

### 3. 文件监控
- 使用定期检查而非实时监控，以节省电量
- 当前设置为每30秒检查一次
- 可根据需要调整检查间隔

### 4. 下载检测
- 应用会自动跳过临时文件（.tmp, .crdownload 等）
- 通过检查文件可读写性判断下载是否完成
- 避免移动未完成的下载文件

### 5. ViewBinding
- 确保在 `build.gradle` 中启用 `viewBinding`
- 使用 `ActivityMainBinding` 等生成的绑定类
- 在 `onDestroy` 中清理 binding 引用

## 与桌面版的区别

1. **监控方式**: Android 版使用定期检查而非文件系统监听
2. **界面设计**: 使用 Material Design 3 适配移动设备
3. **权限管理**: 需要处理 Android 的存储权限系统
4. **后台运行**: 使用前台服务确保服务不被系统杀死
5. **通知系统**: 使用 Android 通知系统显示状态和操作结果

## 待办事项

- [ ] 添加单元测试
- [ ] 支持多语言
- [ ] 添加文件冲突处理策略（覆盖、跳过、重命名）
- [ ] 支持正则表达式规则
- [ ] 添加文件预览功能
- [ ] 支持网络存储监控
- [ ] 添加暗色主题
- [ ] 优化电池使用
- [ ] 添加统计功能（整理文件数量、节省空间等）

## 许可证

本项目仅供学习和个人使用。
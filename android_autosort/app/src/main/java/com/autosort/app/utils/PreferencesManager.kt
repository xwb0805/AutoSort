package com.autosort.app.utils

import android.content.Context
import android.content.SharedPreferences
import com.autosort.app.model.SortRule
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken
import java.io.File

class PreferencesManager(context: Context) {
    private val prefs: SharedPreferences = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
    private val gson = Gson()

    companion object {
        private const val PREFS_NAME = "autosort_prefs"
        private const val KEY_DOWNLOAD_PATH = "download_path"
        private const val KEY_RULES = "rules"
        private const val KEY_MONITORING_ENABLED = "monitoring_enabled"
        private const val KEY_AUTO_START = "auto_start"
        private const val KEY_SHOW_NOTIFICATION = "show_notification"
    }

    var downloadPath: String
        get() = prefs.getString(KEY_DOWNLOAD_PATH, getDefaultDownloadPath()) ?: getDefaultDownloadPath()
        set(value) = prefs.edit().putString(KEY_DOWNLOAD_PATH, value).apply()

    var isMonitoringEnabled: Boolean
        get() = prefs.getBoolean(KEY_MONITORING_ENABLED, false)
        set(value) = prefs.edit().putBoolean(KEY_MONITORING_ENABLED, value).apply()

    var isAutoStartEnabled: Boolean
        get() = prefs.getBoolean(KEY_AUTO_START, false)
        set(value) = prefs.edit().putBoolean(KEY_AUTO_START, value).apply()

    var isShowNotificationEnabled: Boolean
        get() = prefs.getBoolean(KEY_SHOW_NOTIFICATION, true)
        set(value) = prefs.edit().putBoolean(KEY_SHOW_NOTIFICATION, value).apply()

    fun getRules(): List<SortRule> {
        val json = prefs.getString(KEY_RULES, null)
        return if (json != null) {
            val type = object : TypeToken<List<SortRule>>() {}.type
            gson.fromJson(json, type) ?: getDefaultRules()
        } else {
            getDefaultRules()
        }
    }

    fun saveRules(rules: List<SortRule>) {
        val json = gson.toJson(rules)
        prefs.edit().putString(KEY_RULES, json).apply()
    }

    private fun getDefaultDownloadPath(): String {
        return File(android.os.Environment.getExternalStoragePublicDirectory(
            android.os.Environment.DIRECTORY_DOWNLOADS), "").absolutePath
    }

    private fun getDefaultRules(): List<SortRule> {
        return listOf(
            SortRule("documents", "文档", listOf("pdf", "doc", "docx", "txt", "rtf", "odt", "xls", "xlsx", "ppt", "pptx"), "Documents"),
            SortRule("images", "图片", listOf("jpg", "jpeg", "png", "gif", "bmp", "webp", "svg", "ico"), "Images"),
            SortRule("videos", "视频", listOf("mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v"), "Videos"),
            SortRule("audio", "音频", listOf("mp3", "wav", "flac", "aac", "ogg", "wma", "m4a"), "Audio"),
            SortRule("archives", "压缩文件", listOf("zip", "rar", "7z", "tar", "gz", "bz2"), "Archives"),
            SortRule("programs", "程序", listOf("exe", "msi", "apk", "dmg", "deb", "rpm"), "Programs"),
            SortRule("code", "代码", listOf("cpp", "h", "hpp", "java", "py", "js", "ts", "html", "css", "json", "xml", "yaml"), "Code")
        )
    }
}
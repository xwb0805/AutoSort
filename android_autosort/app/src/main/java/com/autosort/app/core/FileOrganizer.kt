package com.autosort.app.core

import android.content.Context
import com.autosort.app.model.LogEntry
import com.autosort.app.model.LogType
import com.autosort.app.model.SortRule
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.io.File
import java.text.SimpleDateFormat
import java.util.*

class FileOrganizer(private val context: Context) {

    private val logEntries = mutableListOf<LogEntry>()
    private val dateFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault())

    suspend fun organizeFiles(sourcePath: String, rules: List<SortRule>): List<LogEntry> {
        return withContext(Dispatchers.IO) {
            val sourceDir = File(sourcePath)
            if (!sourceDir.exists() || !sourceDir.isDirectory) {
                addLog("源路径不存在或不是目录: $sourcePath", LogType.ERROR)
                return@withContext logEntries.toList()
            }

            val files = sourceDir.listFiles() ?: emptyArray()
            var organizedCount = 0

            for (file in files) {
                if (file.isDirectory) continue
                if (isTemporaryFile(file)) continue
                if (!isDownloadComplete(file)) continue

                val extension = file.extension.lowercase()
                val matchingRule = rules.find { rule ->
                    rule.enabled && extension in rule.extensions.map { it.lowercase() }
                }

                if (matchingRule != null) {
                    val targetFolder = File(sourceDir, matchingRule.targetFolder)
                    if (!targetFolder.exists()) {
                        targetFolder.mkdirs()
                    }

                    val targetFile = File(targetFolder, file.name)
                    if (file.renameTo(targetFile)) {
                        addLog("已移动 ${file.name} 到 ${matchingRule.targetFolder}", LogType.SUCCESS)
                        organizedCount++
                    } else {
                        addLog("移动失败: ${file.name}", LogType.ERROR)
                    }
                }
            }

            if (organizedCount == 0) {
                addLog("没有需要整理的文件", LogType.INFO)
            } else {
                addLog("整理完成，共移动 $organizedCount 个文件", LogType.SUCCESS)
            }

            return@withContext logEntries.toList()
        }
    }

    private fun isTemporaryFile(file: File): Boolean {
        val name = file.name.lowercase()
        return name.endsWith(".tmp") ||
               name.endsWith(".crdownload") ||
               name.endsWith(".part") ||
               name.endsWith(".download") ||
               name.startsWith("~") ||
               name.startsWith(".")
    }

    private fun isDownloadComplete(file: File): Boolean {
        return try {
            file.canRead() && file.canWrite()
        } catch (e: Exception) {
            false
        }
    }

    private fun addLog(message: String, type: LogType) {
        logEntries.add(LogEntry(System.currentTimeMillis(), message, type))
    }

    fun getLogs(): List<LogEntry> = logEntries.toList()

    fun clearLogs() = logEntries.clear()
}
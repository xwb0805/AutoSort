package com.autosort.app.model

data class LogEntry(
    val timestamp: Long,
    val message: String,
    val type: LogType = LogType.INFO
)

enum class LogType {
    INFO, SUCCESS, ERROR, WARNING
}
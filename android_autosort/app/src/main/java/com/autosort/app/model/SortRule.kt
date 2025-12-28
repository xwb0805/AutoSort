package com.autosort.app.model

data class SortRule(
    val id: String,
    val name: String,
    val extensions: List<String>,
    val targetFolder: String,
    val enabled: Boolean = true
)
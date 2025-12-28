package com.autosort.app

import android.app.Activity
import android.app.Dialog
import android.content.Intent
import android.os.Bundle
import android.os.Environment
import android.provider.DocumentsContract
import android.view.LayoutInflater
import android.view.Window
import android.widget.CompoundButton
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import com.autosort.app.adapter.LogAdapter
import com.autosort.app.adapter.RulesAdapter
import com.autosort.app.databinding.ActivitySettingsBinding
import com.autosort.app.databinding.DialogEditRuleBinding
import com.autosort.app.model.SortRule
import com.autosort.app.utils.PreferencesManager
import java.io.File
import java.util.UUID

class SettingsActivity : AppCompatActivity() {

    private lateinit var binding: ActivitySettingsBinding
    private lateinit var prefs: PreferencesManager
    private lateinit var logAdapter: LogAdapter

    private var currentRules = mutableListOf<SortRule>()

    private val selectPathLauncher = registerForActivityResult(
        ActivityResultContracts.OpenDocumentTree()
    ) { uri ->
        if (uri != null) {
            contentResolver.takePersistableUriPermission(uri, Intent.FLAG_GRANT_READ_URI_PERMISSION)
            val path = getFilePathFromUri(uri)
            if (path != null) {
                prefs.downloadPath = path
                binding.currentPathText.text = path
            }
        }
    }

    private val importRulesLauncher = registerForActivityResult(
        ActivityResultContracts.OpenDocument()
    ) { uri ->
        if (uri != null) {
            importRules(uri)
        }
    }

    private val exportRulesLauncher = registerForActivityResult(
        ActivityResultContracts.CreateDocument("application/json")
    ) { uri ->
        if (uri != null) {
            exportRules(uri)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivitySettingsBinding.inflate(layoutInflater)
        setContentView(binding.root)

        prefs = PreferencesManager(this)
        currentRules = prefs.getRules().toMutableList()

        setupUI()
    }

    private fun setupUI() {
        binding.currentPathText.text = prefs.downloadPath

        binding.selectPathButton.setOnClickListener {
            selectPathLauncher.launch(null)
        }

        val autoStartSwitch = findViewById<CompoundButton>(R.id.autoStartSwitch)
        autoStartSwitch.isChecked = prefs.isAutoStartEnabled
        autoStartSwitch.setOnCheckedChangeListener { _, isChecked ->
            prefs.isAutoStartEnabled = isChecked
        }

        val notificationSwitch = findViewById<CompoundButton>(R.id.notificationSwitch)
        notificationSwitch.isChecked = prefs.isShowNotificationEnabled
        notificationSwitch.setOnCheckedChangeListener { _, isChecked ->
            prefs.isShowNotificationEnabled = isChecked
        }

        binding.addRuleButton.setOnClickListener {
            showEditRuleDialog(null)
        }

        binding.importRulesButton.setOnClickListener {
            importRulesLauncher.launch(arrayOf("application/json"))
        }

        binding.exportRulesButton.setOnClickListener {
            exportRulesLauncher.launch("autosort_rules.json")
        }

        setupLogList()
    }

    private fun setupLogList() {
        logAdapter = LogAdapter()
        binding.logRecyclerView.apply {
            layoutManager = LinearLayoutManager(this@SettingsActivity)
            adapter = logAdapter
        }

        binding.clearLogButton.setOnClickListener {
            logAdapter.clear()
            Toast.makeText(this, "日志已清空", Toast.LENGTH_SHORT).show()
        }
    }

    private fun showEditRuleDialog(rule: SortRule?) {
        val dialog = Dialog(this)
        dialog.requestWindowFeature(Window.FEATURE_NO_TITLE)
        val dialogBinding = DialogEditRuleBinding.inflate(LayoutInflater.from(dialog.context))
        dialog.setContentView(dialogBinding.root)

        if (rule != null) {
            dialogBinding.ruleNameEditText.setText(rule.name)
            dialogBinding.extensionsEditText.setText(rule.extensions.joinToString(","))
            dialogBinding.targetFolderEditText.setText(rule.targetFolder)
        }

        dialogBinding.cancelButton.setOnClickListener {
            dialog.dismiss()
        }

        dialogBinding.saveButton.setOnClickListener {
            val name = dialogBinding.ruleNameEditText.text.toString().trim()
            val extensions = dialogBinding.extensionsEditText.text.toString().trim()
            val targetFolder = dialogBinding.targetFolderEditText.text.toString().trim()

            if (name.isEmpty() || extensions.isEmpty() || targetFolder.isEmpty()) {
                Toast.makeText(this, "请填写所有字段", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            val extensionList = extensions.split(",").map { it.trim() }.filter { it.isNotEmpty() }

            if (rule != null) {
                val index = currentRules.indexOfFirst { it.id == rule.id }
                if (index >= 0) {
                    currentRules[index] = rule.copy(
                        name = name,
                        extensions = extensionList,
                        targetFolder = targetFolder
                    )
                }
            } else {
                currentRules.add(
                    SortRule(
                        id = UUID.randomUUID().toString(),
                        name = name,
                        extensions = extensionList,
                        targetFolder = targetFolder
                    )
                )
            }

            prefs.saveRules(currentRules)
            dialog.dismiss()
            Toast.makeText(this, "规则已保存", Toast.LENGTH_SHORT).show()
        }

        dialog.show()
    }

    private fun importRules(uri: android.net.Uri) {
        try {
            val content = contentResolver.openInputStream(uri)?.bufferedReader()?.readText()
            if (content != null) {
                val gson = com.google.gson.Gson()
                val type = object : com.google.gson.reflect.TypeToken<List<SortRule>>() {}.type
                val importedRules = gson.fromJson<List<SortRule>>(content, type)
                currentRules.clear()
                currentRules.addAll(importedRules)
                prefs.saveRules(currentRules)
                Toast.makeText(this, "规则已导入", Toast.LENGTH_SHORT).show()
            }
        } catch (e: Exception) {
            Toast.makeText(this, "导入失败: ${e.message}", Toast.LENGTH_SHORT).show()
        }
    }

    private fun exportRules(uri: android.net.Uri) {
        try {
            val gson = com.google.gson.Gson()
            val json = gson.toJson(currentRules)
            contentResolver.openOutputStream(uri)?.use { it.write(json.toByteArray()) }
            Toast.makeText(this, "规则已导出", Toast.LENGTH_SHORT).show()
        } catch (e: Exception) {
            Toast.makeText(this, "导出失败: ${e.message}", Toast.LENGTH_SHORT).show()
        }
    }

    private fun getFilePathFromUri(uri: android.net.Uri): String? {
        var path: String? = null
        if (DocumentsContract.isDocumentUri(this, uri)) {
            val docId = DocumentsContract.getDocumentId(uri)
            val split = docId.split(":")
            val type = split[0]
            if ("primary".equals(type, ignoreCase = true)) {
                path = Environment.getExternalStorageDirectory().path + "/" + split[1]
            }
        }
        return path
    }

    override fun onResume() {
        super.onResume()
        binding.currentPathText.text = prefs.downloadPath
        findViewById<CompoundButton>(R.id.autoStartSwitch).isChecked = prefs.isAutoStartEnabled
        findViewById<CompoundButton>(R.id.notificationSwitch).isChecked = prefs.isShowNotificationEnabled
        currentRules = prefs.getRules().toMutableList()
    }
}
package com.autosort.app

import android.Manifest
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.DocumentsContract
import android.widget.CompoundButton
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.recyclerview.widget.LinearLayoutManager
import com.autosort.app.adapter.RulesAdapter
import com.autosort.app.databinding.ActivityMainBinding
import com.autosort.app.service.FileMonitorService
import com.autosort.app.utils.PreferencesManager

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var prefs: PreferencesManager
    private lateinit var rulesAdapter: RulesAdapter

    private val requestPermissionLauncher = registerForActivityResult(
        ActivityResultContracts.RequestMultiplePermissions()
    ) { permissions ->
        val allGranted = permissions.values.all { it }
        if (allGranted) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                requestManageExternalStorage()
            }
        } else {
            Toast.makeText(this, R.string.permission_required, Toast.LENGTH_LONG).show()
        }
    }

    private val manageStorageLauncher = registerForActivityResult(
        ActivityResultContracts.StartActivityForResult()
    ) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Toast.makeText(this, R.string.permission_required, Toast.LENGTH_LONG).show()
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        prefs = PreferencesManager(this)

        setupUI()
        checkPermissions()
        updateStatus()
    }

    private fun setupUI() {
        binding.downloadPathText.text = prefs.downloadPath

        val monitorSwitch = findViewById<CompoundButton>(R.id.monitorSwitch)
        monitorSwitch.isChecked = prefs.isMonitoringEnabled
        monitorSwitch.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) {
                startMonitoring()
            } else {
                stopMonitoring()
            }
        }

        binding.organizeButton.setOnClickListener {
            organizeFiles()
        }

        binding.settingsButton.setOnClickListener {
            startActivity(Intent(this, SettingsActivity::class.java))
        }

        setupRulesList()
    }

    private fun setupRulesList() {
        val rules = prefs.getRules()
        rulesAdapter = RulesAdapter(
            rules = rules,
            onRuleToggle = { rule ->
                val updatedRules = rules.map {
                    if (it.id == rule.id) it.copy(enabled = !it.enabled) else it
                }
                prefs.saveRules(updatedRules)
            },
            onRuleEdit = { rule ->
                Toast.makeText(this, "编辑功能在设置中实现", Toast.LENGTH_SHORT).show()
            }
        )

        binding.rulesRecyclerView.apply {
            layoutManager = LinearLayoutManager(this@MainActivity)
            adapter = rulesAdapter
        }
    }

    private fun checkPermissions() {
        val permissions = mutableListOf(
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
        )

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            permissions.add(Manifest.permission.POST_NOTIFICATIONS)
        }

        val missingPermissions = permissions.filter {
            ContextCompat.checkSelfPermission(this, it) != PackageManager.PERMISSION_GRANTED
        }

        if (missingPermissions.isNotEmpty()) {
            requestPermissionLauncher.launch(missingPermissions.toTypedArray())
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            requestManageExternalStorage()
        }
    }

    private fun requestManageExternalStorage() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                val intent = Intent(
                    android.provider.Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION,
                    Uri.parse("package:$packageName")
                )
                manageStorageLauncher.launch(intent)
            }
        }
    }

    private fun startMonitoring() {
        prefs.isMonitoringEnabled = true
        val intent = Intent(this, FileMonitorService::class.java)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(intent)
        } else {
            startService(intent)
        }
        updateStatus()
    }

    private fun stopMonitoring() {
        prefs.isMonitoringEnabled = false
        val intent = Intent(this, FileMonitorService::class.java)
        stopService(intent)
        updateStatus()
    }

    private fun organizeFiles() {
        val intent = Intent(this, FileMonitorService::class.java)
        intent.putExtra(FileMonitorService.EXTRA_ORGANIZE_ONCE, true)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(intent)
        } else {
            startService(intent)
        }
    }

    private fun updateStatus() {
        if (prefs.isMonitoringEnabled) {
            binding.statusText.setText(R.string.service_running)
            binding.statusIndicator.setBackgroundResource(R.drawable.status_indicator_on)
        } else {
            binding.statusText.setText(R.string.service_stopped)
            binding.statusIndicator.setBackgroundResource(R.drawable.status_indicator_off)
        }
    }

    override fun onResume() {
        super.onResume()
        binding.downloadPathText.text = prefs.downloadPath
        findViewById<CompoundButton>(R.id.monitorSwitch).isChecked = prefs.isMonitoringEnabled
        updateStatus()
        
        val rules = prefs.getRules()
        rulesAdapter = RulesAdapter(
            rules = rules,
            onRuleToggle = { rule ->
                val updatedRules = rules.map {
                    if (it.id == rule.id) it.copy(enabled = !it.enabled) else it
                }
                prefs.saveRules(updatedRules)
            },
            onRuleEdit = { rule ->
                Toast.makeText(this, "编辑功能在设置中实现", Toast.LENGTH_SHORT).show()
            }
        )
        binding.rulesRecyclerView.adapter = rulesAdapter
    }
}
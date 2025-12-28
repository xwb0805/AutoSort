package com.autosort.app.service

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat
import com.autosort.app.MainActivity
import com.autosort.app.R
import com.autosort.app.core.FileOrganizer
import com.autosort.app.utils.PreferencesManager
import kotlinx.coroutines.*

class FileMonitorService : Service() {

    companion object {
        const val CHANNEL_ID = "AutoSortChannel"
        const val NOTIFICATION_ID = 1
        const val EXTRA_ORGANIZE_ONCE = "organize_once"
    }

    private val serviceScope = CoroutineScope(Dispatchers.IO + SupervisorJob())
    private lateinit var prefs: PreferencesManager
    private lateinit var fileOrganizer: FileOrganizer
    private var monitorJob: Job? = null

    override fun onCreate() {
        super.onCreate()
        prefs = PreferencesManager(this)
        fileOrganizer = FileOrganizer(this)
        createNotificationChannel()
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val organizeOnce = intent?.getBooleanExtra(EXTRA_ORGANIZE_ONCE, false) ?: false

        if (organizeOnce) {
            startForeground(NOTIFICATION_ID, createNotification())
            serviceScope.launch {
                organizeFiles()
                stopSelf()
            }
        } else {
            startForeground(NOTIFICATION_ID, createNotification())
            startMonitoring()
        }

        return START_STICKY
    }

    override fun onBind(intent: Intent?): IBinder? = null

    override fun onDestroy() {
        super.onDestroy()
        stopMonitoring()
        serviceScope.cancel()
    }

    private fun startMonitoring() {
        monitorJob?.cancel()
        monitorJob = serviceScope.launch {
            while (isActive) {
                if (prefs.isMonitoringEnabled) {
                    organizeFiles()
                }
                delay(30000) // 每30秒检查一次
            }
        }
    }

    private fun stopMonitoring() {
        monitorJob?.cancel()
        monitorJob = null
    }

    private suspend fun organizeFiles() {
        val rules = prefs.getRules()
        val downloadPath = prefs.downloadPath
        val logs = fileOrganizer.organizeFiles(downloadPath, rules)
        
        logs.forEach { log ->
            if (prefs.isShowNotificationEnabled) {
                showOrganizeNotification(log.message)
            }
        }
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                CHANNEL_ID,
                "AutoSort 文件监控",
                NotificationManager.IMPORTANCE_LOW
            ).apply {
                description = "AutoSort 文件监控服务通知"
                setShowBadge(false)
            }

            val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.createNotificationChannel(channel)
        }
    }

    private fun createNotification(): Notification {
        val intent = Intent(this, MainActivity::class.java)
        val pendingIntent = PendingIntent.getActivity(
            this,
            0,
            intent,
            PendingIntent.FLAG_IMMUTABLE or PendingIntent.FLAG_UPDATE_CURRENT
        )

        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle(getString(R.string.notification_title))
            .setContentText(getString(R.string.notification_text))
            .setSmallIcon(R.drawable.ic_sort)
            .setContentIntent(pendingIntent)
            .setOngoing(true)
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .build()
    }

    private fun showOrganizeNotification(message: String) {
        val notification = NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("AutoSort")
            .setContentText(message)
            .setSmallIcon(R.drawable.ic_sort)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .build()

        val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
        notificationManager.notify(System.currentTimeMillis().toInt(), notification)
    }
}
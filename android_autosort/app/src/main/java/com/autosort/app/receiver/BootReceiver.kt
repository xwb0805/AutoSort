package com.autosort.app.receiver

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import com.autosort.app.service.FileMonitorService
import com.autosort.app.utils.PreferencesManager

class BootReceiver : BroadcastReceiver() {
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action == Intent.ACTION_BOOT_COMPLETED || 
            intent.action == "android.intent.action.QUICKBOOT_POWERON") {
            
            val prefs = PreferencesManager(context)
            if (prefs.isAutoStartEnabled) {
                val serviceIntent = Intent(context, FileMonitorService::class.java)
                if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                    context.startForegroundService(serviceIntent)
                } else {
                    context.startService(serviceIntent)
                }
            }
        }
    }
}
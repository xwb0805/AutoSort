package com.autosort.app.adapter

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.autosort.app.databinding.ItemLogBinding
import com.autosort.app.model.LogEntry
import java.text.SimpleDateFormat
import java.util.*

class LogAdapter : RecyclerView.Adapter<LogAdapter.LogViewHolder>() {

    private val logs = mutableListOf<LogEntry>()
    private val timeFormat = SimpleDateFormat("HH:mm:ss", Locale.getDefault())

    fun submitList(newLogs: List<LogEntry>) {
        logs.clear()
        logs.addAll(newLogs)
        notifyDataSetChanged()
    }

    fun clear() {
        logs.clear()
        notifyDataSetChanged()
    }

    inner class LogViewHolder(private val binding: ItemLogBinding) :
        RecyclerView.ViewHolder(binding.root) {

        fun bind(log: LogEntry) {
            val time = timeFormat.format(Date(log.timestamp))
            binding.logTime.text = time
            binding.logMessage.text = log.message

            val color = if (log.type == com.autosort.app.model.LogType.ERROR) {
                0xFFFF5252.toInt()
            } else if (log.type == com.autosort.app.model.LogType.SUCCESS) {
                0xFF4CAF50.toInt()
            } else if (log.type == com.autosort.app.model.LogType.WARNING) {
                0xFFFFC107.toInt()
            } else {
                0xFFFFFFFF.toInt()
            }
            binding.logMessage.setTextColor(color)
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): LogViewHolder {
        val binding = ItemLogBinding.inflate(
            LayoutInflater.from(parent.context),
            parent,
            false
        )
        return LogViewHolder(binding)
    }

    override fun onBindViewHolder(holder: LogViewHolder, position: Int) {
        holder.bind(logs[position])
    }

    override fun getItemCount(): Int = logs.size
}
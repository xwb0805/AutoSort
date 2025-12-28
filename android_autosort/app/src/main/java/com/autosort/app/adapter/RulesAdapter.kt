package com.autosort.app.adapter

import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.CompoundButton
import androidx.recyclerview.widget.RecyclerView
import com.autosort.app.R
import com.autosort.app.databinding.ItemRuleBinding
import com.autosort.app.model.SortRule

class RulesAdapter(
    private val rules: List<SortRule>,
    private val onRuleToggle: (SortRule) -> Unit,
    private val onRuleEdit: (SortRule) -> Unit
) : RecyclerView.Adapter<RulesAdapter.RuleViewHolder>() {

    inner class RuleViewHolder(private val binding: ItemRuleBinding) :
        RecyclerView.ViewHolder(binding.root) {

        fun bind(rule: SortRule) {
            val ruleSwitch = binding.root.findViewById<CompoundButton>(R.id.ruleSwitch)
            ruleSwitch.isChecked = rule.enabled
            binding.ruleName.text = rule.name
            binding.ruleExtensions.text = rule.extensions.joinToString(", ")
            binding.ruleTarget.text = "→ ${rule.targetFolder}"

            ruleSwitch.setOnCheckedChangeListener { _, _ ->
                onRuleToggle(rule)
            }

            binding.editButton.setOnClickListener {
                onRuleEdit(rule)
            }
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): RuleViewHolder {
        val binding = ItemRuleBinding.inflate(
            LayoutInflater.from(parent.context),
            parent,
            false
        )
        return RuleViewHolder(binding)
    }

    override fun onBindViewHolder(holder: RuleViewHolder, position: Int) {
        holder.bind(rules[position])
    }

    override fun getItemCount(): Int = rules.size
}
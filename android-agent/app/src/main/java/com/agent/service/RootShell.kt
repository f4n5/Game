package com.agent.service

import java.io.DataOutputStream
import java.util.concurrent.TimeUnit

object RootShell {

    data class Result(val code: Int, val stdout: ByteArray, val stderr: String) {
        val text: String get() = stdout.toString(Charsets.UTF_8)
    }

    fun available(): Boolean = run("id").text.contains("uid=0")

    fun run(cmd: String): Result {
        return try {
            val proc = ProcessBuilder("su").redirectErrorStream(false).start()
            DataOutputStream(proc.outputStream).use { os ->
                os.writeBytes(cmd + "\nexit\n")
                os.flush()
            }
            val out = proc.inputStream.readBytes()
            val err = proc.errorStream.readBytes().toString(Charsets.UTF_8)
            val ok = proc.waitFor(10, TimeUnit.SECONDS)
            if (!ok) proc.destroyForcibly()
            Result(if (ok) proc.exitValue() else -1, out, err)
        } catch (e: Exception) {
            Result(-1, ByteArray(0), e.message ?: "")
        }
    }

    fun readFile(path: String): ByteArray? {
        if (!path.contains("'")) {
            val r = run("cat '$path' 2>/dev/null")
            return if (r.code == 0 && r.stdout.isNotEmpty()) r.stdout else null
        }
        return null
    }

    fun listDir(path: String): List<String> {
        val r = run("ls '$path' 2>/dev/null")
        if (r.code != 0) return emptyList()
        return r.text.split('\n').map { it.trim() }.filter { it.isNotEmpty() }
    }

    fun isDir(path: String): Boolean = run("[ -d '$path' ] && echo yes").text.contains("yes")
}

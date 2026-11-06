# GgWare External — Setup & Build

> Windows x64 · C++17 · ImGui 1.90 · DirectX 11

---

## What's in here

| File | What it does |
|------|--------------|
| `main.cpp` | Win32 window, D3D11 overlay, render loop, INSERT toggle |
| `Config.h` | All feature structs (Aimbot, ESP, World, You…) |
| `Config.cpp` | JSON save/load/import/export via nlohmann/json |
| `Theme.h` | GgWare color palette, ImGui style, custom Toggle/Slider widgets |
| `GUI.h` | All tabs rendered: Combat / Visuals / World / Character / Config / You |
| `CMakeLists.txt` | CMake build |

---

## Dependencies (get these first)

```
git clone https://github.com/ocornut/imgui        ./imgui
```

Download `nlohmann/json.hpp` → put in `./include/json.hpp`:
```
https://github.com/nlohmann/json/releases/latest
```

---

## Build (MSVC)

```bat
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

Or open the `.sln` in Visual Studio and hit Build.

---

## Hooking up your offsets

Drop your offsets from `f4n5/Game` into a new `Offsets.h`:

```cpp
namespace Offsets {
    // Update every Roblox patch
    constexpr uintptr_t DataModel          = 0x0;
    constexpr uintptr_t Players            = 0x0;
    constexpr uintptr_t LocalPlayer        = 0x0;
    constexpr uintptr_t Character          = 0x0;
    constexpr uintptr_t HumanoidRootPart   = 0x0;
    constexpr uintptr_t CFrame             = 0x0;
    constexpr uintptr_t Health             = 0x0;
    constexpr uintptr_t MaxHealth          = 0x0;
    constexpr uintptr_t Name               = 0x0;
    constexpr uintptr_t TeamColor          = 0x0;
    constexpr uintptr_t Camera             = 0x0;
    constexpr uintptr_t ViewMatrix         = 0x0;
    constexpr uintptr_t FoV               = 0x0;
}
```

Then wire them up in a `Memory.h` using `ReadProcessMemory` / `WriteProcessMemory`
on `RobloxPlayerBeta.exe` via `OpenProcess(PROCESS_ALL_ACCESS, ...)`.

---

## Keyboard

| Key | Action |
|-----|--------|
| `INSERT` | Toggle menu open/close |
| When menu is open | Window is click-through OFF (you interact with it) |
| When menu is closed | Window is click-through ON (fully transparent) |

---

## Music tab (YouTube / Spotify)

The music player in the `You` tab stores URLs and playlist.
To actually *play* audio you have two clean routes:

1. **Inject a WebView2 / CEF instance** into the overlay and load a YouTube
   embed URL — works for both YouTube and Spotify web player.
2. **yt-dlp + ffplay pipe** — shell out `yt-dlp -o - <url> | ffplay -i -`,
   works for YouTube; Spotify needs premium for stream URLs.

The UI is fully wired; you just need to call your audio backend when
`y.music.isPlaying` flips to true.

---

## Config files

Saved to `%APPDATA%\GgWare\configs\<name>.json`.
Import/Export buttons in the Config tab use standard file paths.
JSON is human-readable — you can edit them by hand.

---

## Notes

- **Anticheat**: This is an external overlay. It does not inject into
  Roblox's process. For memory reads you use RPM which is significantly
  less detectable than injection, but no guarantee.
- **Offsets**: Roblox updates frequently. Keep `Offsets.h` fresh.
  Use the pattern/signature scanner in your repo for auto-updating.
- **Silent Aim**: Requires a camera-redirect or CFrame write approach.
  The UI for it is complete; the implementation depends on your write
  primitive.

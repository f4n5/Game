// ─────────────────────────────────────────────────────────
//  GgWare Config.cpp  — JSON save / load
//  Dep: nlohmann/json (header-only)
//  Build: add nlohmann/json.hpp to include path
// ─────────────────────────────────────────────────────────

#include "Config.h"
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <shlobj.h>

namespace fs = std::filesystem;

namespace GgWare {

std::string ConfigManager::configDir = "";

// ── Helpers ───────────────────────────────────────────────
static std::string GetConfigDir() {
    if (!ConfigManager::configDir.empty()) return ConfigManager::configDir;
    char path[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path);
    ConfigManager::configDir = std::string(path) + "\\GgWare\\configs\\";
    fs::create_directories(ConfigManager::configDir);
    return ConfigManager::configDir;
}

// ── Serialization helpers ─────────────────────────────────
using json = nlohmann::json;

static json ColToJson(const Color4& c) {
    return {c.r, c.g, c.b, c.a};
}
static Color4 ColFromJson(const json& j) {
    return {j[0], j[1], j[2], j[3]};
}

#define SV(j, s, f)  j[#f] = s.f
#define LV(j, s, f)  if(j.contains(#f)) s.f = j[#f]
#define SC(j, s, f)  j[#f] = ColToJson(s.f)
#define LC(j, s, f)  if(j.contains(#f)) s.f = ColFromJson(j[#f])

static json SerAimbot(const AimbotCfg& a) {
    json j;
    SV(j,a,enabled); SV(j,a,teamCheck); SV(j,a,visibleCheck);
    SV(j,a,healthCheck); SV(j,a,stickyAim);
    SV(j,a,distance); SV(j,a,sensitivity);
    SV(j,a,hitPart); SV(j,a,aimType); SV(j,a,rageMethod);
    SV(j,a,resolver);
    return j;
}
static void DeserAimbot(const json& j, AimbotCfg& a) {
    LV(j,a,enabled); LV(j,a,teamCheck); LV(j,a,visibleCheck);
    LV(j,a,healthCheck); LV(j,a,stickyAim);
    LV(j,a,distance); LV(j,a,sensitivity);
    LV(j,a,hitPart); LV(j,a,aimType); LV(j,a,rageMethod);
    LV(j,a,resolver);
}

static json SerSmooth(const SmoothBotCfg& s) {
    json j;
    SV(j,s,enabled); SV(j,s,smoothX); SV(j,s,smoothY);
    SV(j,s,predictionScale); SV(j,s,humanize); SV(j,s,humanizeAmount);
    SV(j,s,fov); SV(j,s,showFov);
    SC(j,s,fovColor);
    return j;
}
static void DeserSmooth(const json& j, SmoothBotCfg& s) {
    LV(j,s,enabled); LV(j,s,smoothX); LV(j,s,smoothY);
    LV(j,s,predictionScale); LV(j,s,humanize); LV(j,s,humanizeAmount);
    LV(j,s,fov); LV(j,s,showFov);
    LC(j,s,fovColor);
}

static json SerTrigger(const TriggerBotCfg& t) {
    json j;
    SV(j,t,enabled); SV(j,t,visibleCheck); SV(j,t,teamCheck);
    SV(j,t,hitboxMul); SV(j,t,delayMs); SV(j,t,releaseMs); SV(j,t,key);
    return j;
}
static void DeserTrigger(const json& j, TriggerBotCfg& t) {
    LV(j,t,enabled); LV(j,t,visibleCheck); LV(j,t,teamCheck);
    LV(j,t,hitboxMul); LV(j,t,delayMs); LV(j,t,releaseMs); LV(j,t,key);
}

static json SerSilent(const SilentAimCfg& s) {
    json j;
    SV(j,s,enabled); SV(j,s,teamCheck); SV(j,s,visibleCheck);
    SV(j,s,healthCheck); SV(j,s,stickyAim);
    SV(j,s,distance); SV(j,s,hitPart); SV(j,s,method);
    return j;
}
static void DeserSilent(const json& j, SilentAimCfg& s) {
    LV(j,s,enabled); LV(j,s,teamCheck); LV(j,s,visibleCheck);
    LV(j,s,healthCheck); LV(j,s,stickyAim);
    LV(j,s,distance); LV(j,s,hitPart); LV(j,s,method);
}

static json SerESP(const ESPCfg& e) {
    json j;
    SV(j,e,enabled); SV(j,e,teamCheck); SV(j,e,visibleCheck);
    SV(j,e,teamBasedColor); SV(j,e,textGradient); SV(j,e,textBackground);
    SV(j,e,outline); SV(j,e,glow); SV(j,e,selfESP);
    SV(j,e,sizingType); SV(j,e,renderDistance);
    SV(j,e,boxEnabled); SV(j,e,fillBox); SV(j,e,boxType);
    SC(j,e,boxColorEnemy); SC(j,e,boxColorTeam); SC(j,e,boxFillColor);
    SV(j,e,nameEnabled); SV(j,e,nameType);
    SC(j,e,nameColor);
    SV(j,e,showDistance); SV(j,e,showItem); SV(j,e,showSkeleton);
    SV(j,e,headDot); SV(j,e,headDotGlow); SV(j,e,profilePicture);
    SC(j,e,skeletonColor);
    SV(j,e,healthBar); SV(j,e,healthBased); SV(j,e,healthText);
    SV(j,e,textPos);
    SC(j,e,healthBarColor);
    SV(j,e,chamsEnabled); SV(j,e,chamsFilled); SV(j,e,chamsMode); SV(j,e,chamsRenderType);
    SV(j,e,tracerEnabled); SV(j,e,tracerOrigin);
    SC(j,e,tracerColor);
    return j;
}
static void DeserESP(const json& j, ESPCfg& e) {
    LV(j,e,enabled); LV(j,e,teamCheck); LV(j,e,visibleCheck);
    LV(j,e,teamBasedColor); LV(j,e,textGradient); LV(j,e,textBackground);
    LV(j,e,outline); LV(j,e,glow); LV(j,e,selfESP);
    LV(j,e,sizingType); LV(j,e,renderDistance);
    LV(j,e,boxEnabled); LV(j,e,fillBox); LV(j,e,boxType);
    LC(j,e,boxColorEnemy); LC(j,e,boxColorTeam); LC(j,e,boxFillColor);
    LV(j,e,nameEnabled); LV(j,e,nameType);
    LC(j,e,nameColor);
    LV(j,e,showDistance); LV(j,e,showItem); LV(j,e,showSkeleton);
    LV(j,e,headDot); LV(j,e,headDotGlow); LV(j,e,profilePicture);
    LC(j,e,skeletonColor);
    LV(j,e,healthBar); LV(j,e,healthBased); LV(j,e,healthText);
    LV(j,e,textPos);
    LC(j,e,healthBarColor);
    LV(j,e,chamsEnabled); LV(j,e,chamsFilled); LV(j,e,chamsMode); LV(j,e,chamsRenderType);
    LV(j,e,tracerEnabled); LV(j,e,tracerOrigin);
    LC(j,e,tracerColor);
}

static json SerConfig(const FullConfig& cfg) {
    json j;
    j["name"] = cfg.name;

    j["combat"]["aimbot"]    = SerAimbot(cfg.combat.aimbot);
    j["combat"]["smoothBot"] = SerSmooth(cfg.combat.smoothBot);
    j["combat"]["triggerBot"]= SerTrigger(cfg.combat.triggerBot);
    j["combat"]["silentAim"] = SerSilent(cfg.combat.silentAim);

    j["visuals"]["esp"] = SerESP(cfg.visuals.esp);
    // crosshair & radar left as exercise; same pattern

    // world
    auto& w = cfg.world;
    j["world"]["customSky"]=w.customSky; j["world"]["skyPreset"]=w.skyPreset;
    j["world"]["skyColorTop"]=ColToJson(w.skyColorTop);
    j["world"]["skyColorBot"]=ColToJson(w.skyColorBot);
    j["world"]["customAmbient"]=w.customAmbient;
    j["world"]["ambientColor"]=ColToJson(w.ambientColor);
    j["world"]["ambientBrightness"]=w.ambientBrightness;
    j["world"]["customLighting"]=w.customLighting;
    j["world"]["brightness"]=w.brightness;
    j["world"]["contrast"]=w.contrast;
    j["world"]["saturation"]=w.saturation;
    j["world"]["gamma"]=w.gamma;
    j["world"]["customFOV"]=w.customFOV;
    j["world"]["fov"]=w.fov;

    // character
    auto& ch = cfg.character;
    j["character"]["hitboxExtender"]["enabled"]    = ch.hitboxExtender.enabled;
    j["character"]["hitboxExtender"]["hitboxSize"] = ch.hitboxExtender.hitboxSize;
    j["character"]["hitboxExtender"]["type"]       = ch.hitboxExtender.type;
    j["character"]["movement"]["noclip"]    = ch.movement.noclip;
    j["character"]["movement"]["infJump"]   = ch.movement.infJump;
    j["character"]["movement"]["speedEnabled"]  = ch.movement.speedEnabled;
    j["character"]["movement"]["speedAmount"]   = ch.movement.speedAmount;
    j["character"]["movement"]["flight"]    = ch.movement.flight;
    j["character"]["movement"]["flyAmount"] = ch.movement.flyAmount;
    j["character"]["movement"]["jumpPower"] = ch.movement.jumpPower;
    j["character"]["desync"]["enabled"]     = ch.desync.enabled;
    j["character"]["desync"]["interval"]    = ch.desync.desyncInterval;

    // you
    auto& y = cfg.you;
    j["you"]["antiAFK"]       = y.antiAFK;
    j["you"]["rejoinOnKick"]  = y.rejoinOnKick;
    j["you"]["unlockFPS"]     = y.unlockFPS;
    j["you"]["targetFPS"]     = y.targetFPS;
    j["you"]["pingSpoofer"]   = y.pingSpoofer;
    j["you"]["fakePing"]      = y.fakePing;
    j["you"]["charSpoofer"]["enabled"]     = y.charSpoofer.enabled;
    j["you"]["charSpoofer"]["spoofedName"] = y.charSpoofer.spoofedName;
    j["you"]["charSpoofer"]["spoofedId"]   = y.charSpoofer.spoofedId;
    j["you"]["music"]["enabled"]  = y.music.enabled;
    j["you"]["music"]["volume"]   = y.music.volume;
    j["you"]["music"]["loop"]     = y.music.loop;
    j["you"]["music"]["shuffle"]  = y.music.shuffle;
    j["you"]["music"]["playlist"] = y.music.playlist;
    return j;
}

static void DeserConfig(const json& j, FullConfig& cfg) {
    if (j.contains("name")) cfg.name = j["name"];
    if (j.contains("combat")) {
        auto& c = j["combat"];
        if (c.contains("aimbot"))    DeserAimbot(c["aimbot"],    cfg.combat.aimbot);
        if (c.contains("smoothBot")) DeserSmooth(c["smoothBot"], cfg.combat.smoothBot);
        if (c.contains("triggerBot"))DeserTrigger(c["triggerBot"],cfg.combat.triggerBot);
        if (c.contains("silentAim")) DeserSilent(c["silentAim"], cfg.combat.silentAim);
    }
    if (j.contains("visuals") && j["visuals"].contains("esp"))
        DeserESP(j["visuals"]["esp"], cfg.visuals.esp);
    // world, character, you: same approach
}

// ── ConfigManager implementation ──────────────────────────

void ConfigManager::Save(const FullConfig& cfg, const std::string& filename) {
    std::string dir  = GetConfigDir();
    std::ofstream f(dir + filename);
    f << SerConfig(cfg).dump(4);
}

void ConfigManager::Load() {
    std::string dir = GetConfigDir();
    if (!fs::exists(dir)) return;
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() != ".json") continue;
        try {
            std::ifstream f(entry.path());
            json j = json::parse(f);
            FullConfig cfg;
            DeserConfig(j, cfg);
            configs.push_back(cfg);
        } catch (...) {}
    }
    if (configs.empty()) {
        configs.push_back(FullConfig{});
        configs.back().name = "Default";
    }
}

bool ConfigManager::Import(const std::string& path) {
    try {
        std::ifstream f(path);
        if (!f.is_open()) return false;
        json j = json::parse(f);
        FullConfig cfg;
        DeserConfig(j, cfg);
        configs.push_back(cfg);
        activeIdx = (int)configs.size() - 1;
        return true;
    } catch (...) { return false; }
}

bool ConfigManager::Export(const std::string& path, int idx) {
    if (idx < 0 || idx >= (int)configs.size()) return false;
    try {
        std::ofstream f(path);
        f << SerConfig(configs[idx]).dump(4);
        return true;
    } catch (...) { return false; }
}

void ConfigManager::New(const std::string& name) {
    FullConfig cfg;
    cfg.name = name;
    configs.push_back(cfg);
    activeIdx = (int)configs.size() - 1;
    Save(cfg, name + ".json");
}

void ConfigManager::Delete(int idx) {
    if (idx < 0 || idx >= (int)configs.size()) return;
    std::string dir  = GetConfigDir();
    std::string file = dir + configs[idx].name + ".json";
    if (fs::exists(file)) fs::remove(file);
    configs.erase(configs.begin() + idx);
    if (activeIdx >= (int)configs.size()) activeIdx = (int)configs.size() - 1;
}

} // namespace GgWare

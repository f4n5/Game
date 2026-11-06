#pragma once
#include <string>
#include <vector>
#include <array>
#include <nlohmann/json.hpp>

// ─────────────────────────────────────────────────────────
//  GgWare Config Structure
//  Engine: Roblox External (Windows x64)
//  Lang:   C++17 / ImGui 1.90 / Win32
// ─────────────────────────────────────────────────────────

namespace GgWare {

    // ── Helpers ──────────────────────────────────────────
    struct Color4 {
        float r, g, b, a;
        Color4(float r=1,float g=0,float b=1,float a=1):r(r),g(g),b(b),a(a){}
        ImVec4 ToImVec4() const { return {r,g,b,a}; }
        ImU32  ToImU32()  const { return IM_COL32((int)(r*255),(int)(g*255),(int)(b*255),(int)(a*255)); }
    };

    // ── Combat ───────────────────────────────────────────
    struct AimbotCfg {
        bool  enabled         = false;
        bool  teamCheck       = true;
        bool  visibleCheck    = true;
        bool  healthCheck     = false;
        bool  stickyAim       = false;
        float distance        = 510.f;
        float sensitivity     = 2.0f;
        int   hitPart         = 0;   // 0=Head 1=Torso 2=Closest
        int   aimType         = 0;   // 0=Mouse 1=Memory
        int   rageMethod      = 0;   // 0=CamTP 1=Spinbot 2=Instant
        bool  resolver        = false;
    };

    struct SmoothBotCfg {
        bool  enabled         = false;
        float smoothX         = 5.0f;
        float smoothY         = 5.0f;
        float predictionScale = 1.0f;
        bool  humanize        = true;
        float humanizeAmount  = 0.3f;
        int   fov             = 90;
        Color4 fovColor       = {1,0,1,0.6f};
        bool  showFov         = true;
    };

    struct TriggerBotCfg {
        bool  enabled         = false;
        bool  visibleCheck    = true;
        bool  teamCheck       = true;
        float hitboxMul       = 1.0f;
        int   delayMs         = 1;
        int   releaseMs       = 10;
        int   key             = 'Q';
    };

    struct SilentAimCfg {
        bool  enabled         = false;
        bool  teamCheck       = true;
        bool  visibleCheck    = false;
        bool  healthCheck     = false;
        bool  stickyAim       = true;
        float distance        = 10000.f;
        int   hitPart         = 0;   // 0=Head 1=Torso
        int   method          = 0;   // 0=Experimental 1=Legacy
    };

    struct CombatCfg {
        AimbotCfg    aimbot;
        SmoothBotCfg smoothBot;
        TriggerBotCfg triggerBot;
        SilentAimCfg silentAim;
    };

    // ── Visuals ──────────────────────────────────────────
    struct ESPCfg {
        bool   enabled        = false;
        bool   teamCheck      = true;
        bool   visibleCheck   = true;
        bool   teamBasedColor = true;
        bool   textGradient   = false;
        bool   textBackground = false;
        bool   outline        = true;
        bool   glow           = false;
        bool   selfESP        = false;
        int    sizingType     = 0;    // 0=Bounding 1=Fixed
        float  renderDistance = 3115.f;

        // Box
        bool   boxEnabled     = true;
        bool   fillBox        = false;
        int    boxType        = 0;    // 0=Corners 1=Full
        Color4 boxColorEnemy  = {1,0,0,1};
        Color4 boxColorTeam   = {0,0,1,1};
        Color4 boxFillColor   = {1,0,0,0.15f};

        // Name
        bool   nameEnabled    = true;
        int    nameType       = 0;    // 0=Name 1=Display
        Color4 nameColor      = {1,1,1,1};

        // Indicators
        bool   showDistance   = true;
        bool   showItem       = true;
        bool   showSkeleton   = true;
        bool   headDot        = false;
        bool   headDotGlow    = false;
        bool   profilePicture = false;

        // Health
        bool   healthBar      = true;
        bool   healthBased    = false;
        bool   healthText     = false;
        int    textPos        = 0;    // 0=AboveName
        Color4 healthBarColor = {0,1,0,1};

        // Chams
        bool   chamsEnabled   = false;
        bool   chamsFilled    = false;
        int    chamsMode      = 0;    // 0=Default 1=Wireframe
        int    chamsRenderType= 0;    // 0=Static 1=Dynamic

        // Tracer
        bool   tracerEnabled  = false;
        int    tracerOrigin   = 0;    // 0=Top 1=Bottom 2=Middle

        Color4 skeletonColor  = {1,1,1,1};
        Color4 tracerColor    = {1,0,1,1};
    };

    struct CrosshairCfg {
        bool   enabled        = false;
        int    style          = 0;    // 0=Cross 1=Circle 2=Dot
        float  size           = 8.f;
        float  gap            = 4.f;
        float  thickness      = 1.5f;
        bool   dot            = false;
        Color4 color          = {1,1,1,1};
    };

    struct RadarCfg {
        bool   enabled        = false;
        float  size           = 150.f;
        float  range          = 200.f;
        Color4 bgColor        = {0,0,0,0.6f};
        Color4 enemyColor     = {1,0,0,1};
        Color4 teamColor      = {0,0,1,1};
        Color4 selfColor      = {0,1,0,1};
    };

    struct VisualsCfg {
        ESPCfg       esp;
        CrosshairCfg crosshair;
        RadarCfg     radar;
    };

    // ── World ────────────────────────────────────────────
    struct WorldCfg {
        bool   customSky      = false;
        int    skyPreset      = 0;   // 0=Default 1=Night 2=Sunset 3=Custom
        Color4 skyColorTop    = {0.1f,0.1f,0.2f,1};
        Color4 skyColorBot    = {0.3f,0.3f,0.4f,1};

        bool   customAmbient  = false;
        Color4 ambientColor   = {0.5f,0.5f,0.5f,1};
        float  ambientBrightness = 1.0f;

        bool   customLighting = false;
        float  brightness     = 1.0f;
        float  contrast       = 1.0f;
        float  saturation     = 1.0f;
        float  gamma          = 1.0f;
        Color4 lightColor     = {1,1,1,1};

        bool   customFOV      = false;
        float  fov            = 70.f;
    };

    // ── Character ────────────────────────────────────────
    struct HitboxExtenderCfg {
        bool  enabled         = false;
        bool  visualize       = false;
        bool  teamCheck       = true;
        bool  healthCheck     = false;
        float hitboxSize      = 20.f;
        int   type            = 0;   // 0=Old 1=New
    };

    struct TargetHoverCfg {
        bool  enabled         = false;
        bool  displayCircle   = false;
        int   targetMethod    = 0;   // 0=ClosestToMouse 1=ClosestToCenter
        float radius          = 10.f;
        float speed           = 5.f;
    };

    struct MovementCfg {
        bool  noclip          = false;
        bool  infJump         = false;
        bool  clickTP         = false;
        bool  speedEnabled    = false;
        int   speedMethod     = 0;   // 0=Velocity 1=Walkspeed
        float speedAmount     = 1.f;
        bool  flight          = false;
        int   flyMethod       = 0;
        float flyAmount       = 1.f;
        bool  jumpEnabled     = false;
        float jumpPower       = 50.f;
        bool  floatEnabled    = false;
        float floatHeight     = 10.f;
    };

    struct DesyncCfg {
        bool  enabled         = false;
        bool  removeWalkAnim  = false;
        bool  displayServerPos= false;
        bool  resetOnOff      = false;
        bool  useTick         = false;
        int   desyncInterval  = 10;
    };

    struct CharacterCfg {
        HitboxExtenderCfg hitboxExtender;
        TargetHoverCfg    targetHover;
        MovementCfg       movement;
        DesyncCfg         desync;
    };

    // ── You (Misc) ───────────────────────────────────────
    struct CharacterSpooferCfg {
        bool        enabled       = false;
        std::string spoofedName   = "";
        std::string spoofedId     = "";
        bool        spoofTeam     = false;
        std::string spoofedTeam   = "";
    };

    struct MusicCfg {
        bool        enabled       = false;
        std::string currentUrl    = "";
        bool        isPlaying     = false;
        float       volume        = 0.5f;
        int         source        = 0;   // 0=YouTube 1=Spotify
        // Playlist
        std::vector<std::string> playlist;
        int         currentIndex  = 0;
        bool        loop          = false;
        bool        shuffle       = false;
    };

    struct YouCfg {
        CharacterSpooferCfg charSpoofer;
        MusicCfg            music;
        // Extra misc
        bool   antiAFK        = false;
        bool   rejoinOnKick   = false;
        bool   customCursor   = false;
        bool   hideUI         = false;
        bool   unlockFPS      = false;
        int    targetFPS      = 144;
        bool   pingSpoofer    = false;
        int    fakePing       = 20;
    };

    // ── Full Config ──────────────────────────────────────
    struct FullConfig {
        std::string name   = "Default";
        CombatCfg   combat;
        VisualsCfg  visuals;
        WorldCfg    world;
        CharacterCfg character;
        YouCfg      you;
    };

    // ── Config Manager ───────────────────────────────────
    class ConfigManager {
    public:
        std::vector<FullConfig> configs;
        int                     activeIdx = 0;
        static std::string      configDir;

        FullConfig& Active() { return configs[activeIdx]; }

        void Init() {
            configs.push_back(FullConfig{});
            configs.back().name = "Default";
            Load();
        }

        void Save(const FullConfig& cfg, const std::string& filename);
        void Load();
        bool Import(const std::string& path);
        bool Export(const std::string& path, int idx);
        void New(const std::string& name);
        void Delete(int idx);
    };

} // namespace GgWare

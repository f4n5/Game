#pragma once
#include "Config.h"
#include "Theme.h"
#include <imgui.h>
#include <string>

// ─────────────────────────────────────────────────────────
//  GgWare GUI — All tabs rendered here
//  Call GUI::Render() once per frame after ImGui::NewFrame()
// ─────────────────────────────────────────────────────────

namespace GgWare {

extern ConfigManager g_cfgMgr;
extern bool          g_menuOpen;

class GUI {
public:
    // ── State ─────────────────────────────────────────────
    int  m_mainTab    = 0;   // 0=Combat 1=Visuals 2=World 3=Character 4=Config 5=You
    int  m_combatSub  = 0;   // 0=Aimbot 1=SmoothBot 2=TriggerBot 3=SilentAim
    int  m_visualsSub = 0;   // 0=ESP 1=Crosshair 2=Radar
    char m_newCfgName[64]  = "New Config";
    char m_importPath[256] = "";
    char m_songUrl[512]    = "";
    char m_spoofName[64]   = "";
    char m_spoofId[64]     = "";

    // ── Entry point ───────────────────────────────────────
    void Render() {
        if (!g_menuOpen) return;

        FullConfig& cfg = g_cfgMgr.Active();

        // Center on screen once
        static bool first = true;
        if (first) {
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos({center.x - Layout::WindowW * 0.5f,
                                     center.y - Layout::WindowH * 0.5f},
                                    ImGuiCond_Always);
            first = false;
        }

        ImGui::SetNextWindowSize({Layout::WindowW, Layout::WindowH}, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(1.f);
        ImGuiWindowFlags wf = ImGuiWindowFlags_NoResize |
                              ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, Col::BgPanel);
        ImGui::PushStyleColor(ImGuiCol_Border,   Col::BorderAccent);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

        ImGui::Begin("##GgWare", nullptr, wf | ImGuiWindowFlags_NoTitleBar);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        DrawTitleBar();
        DrawSidebar();

        // Content area
        ImGui::SetCursorPos({Layout::SidebarW + 1.f, Layout::TabBarH + 2.f});
        ImVec2 avail = {Layout::WindowW - Layout::SidebarW - 2.f,
                        Layout::WindowH - Layout::TabBarH - 4.f};
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Col::BgDeep);
        ImGui::BeginChild("##content", avail, false);

        switch (m_mainTab) {
            case 0: DrawCombat(cfg);    break;
            case 1: DrawVisuals(cfg);   break;
            case 2: DrawWorld(cfg);     break;
            case 3: DrawCharacter(cfg); break;
            case 4: DrawConfig();       break;
            case 5: DrawYou(cfg);       break;
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::End();
    }

private:

    // ── Title bar ─────────────────────────────────────────
    void DrawTitleBar() {
        ImDrawList* dl  = ImGui::GetWindowDrawList();
        ImVec2      wp  = ImGui::GetWindowPos();
        float       w   = Layout::WindowW;
        float       h   = Layout::TabBarH;

        dl->AddRectFilled(wp, {wp.x+w, wp.y+h}, IM_COL32(10,10,16,255), Layout::WindowRounding, ImDrawFlags_RoundCornersTop);
        // Accent line under title bar
        dl->AddLine({wp.x, wp.y+h}, {wp.x+w, wp.y+h}, ImGui::ColorConvertFloat4ToU32(Col::Accent), 1.f);

        // Logo text
        ImGui::SetCursorPos({12.f, 8.f});
        ImGui::PushStyleColor(ImGuiCol_Text, Col::Accent);
        ImGui::Text("GgWare");
        ImGui::PopStyleColor();

        // Config name
        ImGui::SameLine();
        ImGui::SetCursorPosX(80.f);
        ImGui::PushStyleColor(ImGuiCol_Text, Col::TextDim);
        ImGui::Text("// %s", g_cfgMgr.Active().name.c_str());
        ImGui::PopStyleColor();

        // "X" close button
        ImGui::SameLine(Layout::WindowW - 32.f);
        ImGui::PushStyleColor(ImGuiCol_Button,        {0,0,0,0});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.6f,0,0,1});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {0.9f,0,0,1});
        if (ImGui::SmallButton(" X ")) g_menuOpen = false;
        ImGui::PopStyleColor(3);
    }

    // ── Sidebar tabs ──────────────────────────────────────
    void DrawSidebar() {
        ImVec2 wp = ImGui::GetWindowPos();
        float  h  = Layout::WindowH - Layout::TabBarH;
        float  w  = Layout::SidebarW;
        float  ty = wp.y + Layout::TabBarH;

        ImGui::GetWindowDrawList()->AddRectFilled(
            {wp.x, ty}, {wp.x+w, ty+h},
            IM_COL32(12,12,18,255));
        ImGui::GetWindowDrawList()->AddLine(
            {wp.x+w, ty}, {wp.x+w, ty+h},
            ImGui::ColorConvertFloat4ToU32(Col::Border));

        ImGui::SetCursorPos({0.f, Layout::TabBarH + 10.f});
        ImGui::BeginChild("##sidebar", {w, h - 10.f}, false);

        const char* tabs[] = {
            " Combat",
            " Visuals",
            " World",
            " Character",
            " Config",
            " You"
        };
        const char* icons[] = { "X","V","W","C","S","Y" }; // placeholders

        for (int i = 0; i < 6; i++) {
            bool sel = (m_mainTab == i);
            if (sel) {
                ImGui::PushStyleColor(ImGuiCol_Button, Col::AccentDim);
                ImGui::PushStyleColor(ImGuiCol_Text,   Col::Accent);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, {0,0,0,0});
                ImGui::PushStyleColor(ImGuiCol_Text,   Col::TextNormal);
            }
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, {0.1f, 0.5f});
            if (ImGui::Button(tabs[i], {w - 2.f, 36.f})) m_mainTab = i;
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(2);

            if (sel) {
                // accent left bar
                ImVec2 p = ImGui::GetItemRectMin();
                ImGui::GetWindowDrawList()->AddRectFilled(
                    {p.x, p.y}, {p.x + 3.f, p.y + 36.f},
                    ImGui::ColorConvertFloat4ToU32(Col::Accent));
            }
            ImGui::Spacing();
        }

        // version watermark at bottom
        ImGui::SetCursorPosY(h - 36.f);
        ImGui::PushStyleColor(ImGuiCol_Text, Col::TextDim);
        ImGui::Text("  v1.0 | GgWare");
        ImGui::PopStyleColor();

        ImGui::EndChild();
    }

    // ─────────────────────────────────────────────────────
    //  COMBAT
    // ─────────────────────────────────────────────────────
    void DrawCombat(FullConfig& cfg) {
        // Sub-tabs
        SubTabBar({"Aimbot","SmoothBot","TriggerBot","Silent Aim"}, m_combatSub);
        ImGui::Separator();
        ImGui::Spacing();

        switch (m_combatSub) {
            case 0: DrawAimbot(cfg.combat.aimbot);       break;
            case 1: DrawSmoothBot(cfg.combat.smoothBot); break;
            case 2: DrawTriggerBot(cfg.combat.triggerBot);break;
            case 3: DrawSilentAim(cfg.combat.silentAim); break;
        }
    }

    void DrawAimbot(AimbotCfg& a) {
        TwoCol([&]{
            // Left column
            SectionHeader("General");
            Toggle("Enabled",      &a.enabled);
            Toggle("Team Check",   &a.teamCheck);
            Toggle("Visible Check",&a.visibleCheck);
            Toggle("Health Check", &a.healthCheck);
            Toggle("Sticky Aim",   &a.stickyAim);
            ImGui::Spacing();
            SectionHeader("Targeting");
            SliderF("Distance",    &a.distance,    50.f, 2000.f, "%.0f");
            SliderF("Sensitivity", &a.sensitivity, 0.1f, 10.f);
            ImGui::Spacing();
            const char* hitParts[] = {"Head","Torso","Closest"};
            Combo("Hit Part", &a.hitPart, hitParts, 3);
        }, [&]{
            // Right column
            SectionHeader("Aim Type");
            const char* aimTypes[] = {"Mouse","Memory"};
            Combo("Aim Type", &a.aimType, aimTypes, 2);
            ImGui::Spacing();
            SectionHeader("Rage");
            const char* rageMethods[] = {"Camera Teleport","Spinbot","Instant"};
            Combo("Rage Method", &a.rageMethod, rageMethods, 3);
            ImGui::Spacing();
            SectionHeader("Misc");
            Toggle("Resolver", &a.resolver);
        });
    }

    void DrawSmoothBot(SmoothBotCfg& s) {
        TwoCol([&]{
            SectionHeader("General");
            Toggle("Enabled",    &s.enabled);
            Toggle("Humanize",   &s.humanize);
            ImGui::Spacing();
            SectionHeader("Smoothing");
            SliderF("Smooth X",         &s.smoothX,         1.f, 20.f);
            SliderF("Smooth Y",         &s.smoothY,         1.f, 20.f);
            SliderF("Prediction Scale", &s.predictionScale, 0.f, 5.f);
            SliderF("Humanize Amount",  &s.humanizeAmount,  0.f, 1.f);
        }, [&]{
            SectionHeader("FOV Circle");
            SliderI("FOV",   &s.fov, 10, 360);
            Toggle("Show FOV", &s.showFov);
            ImGui::Spacing();
            ColorEdit("FOV Color", &s.fovColor);
        });
    }

    void DrawTriggerBot(TriggerBotCfg& t) {
        TwoCol([&]{
            SectionHeader("General");
            Toggle("Enabled",      &t.enabled);
            Toggle("Visible Check",&t.visibleCheck);
            Toggle("Team Check",   &t.teamCheck);
            ImGui::Spacing();
            SectionHeader("Timing");
            SliderI("Delay (ms)",   &t.delayMs,   0, 500);
            SliderI("Release (ms)", &t.releaseMs, 0, 500);
        }, [&]{
            SectionHeader("Settings");
            SliderF("Hitbox Mul", &t.hitboxMul, 0.5f, 5.f);
            ImGui::Spacing();
            Keybind("Trigger Key", &t.key);
        });
    }

    void DrawSilentAim(SilentAimCfg& s) {
        TwoCol([&]{
            SectionHeader("General");
            Toggle("Enabled",      &s.enabled);
            Toggle("Team Check",   &s.teamCheck);
            Toggle("Visible Check",&s.visibleCheck);
            Toggle("Health Check", &s.healthCheck);
            Toggle("Sticky Aim",   &s.stickyAim);
            ImGui::Spacing();
            SliderF("Distance", &s.distance, 100.f, 10000.f, "%.0f");
        }, [&]{
            SectionHeader("Targeting");
            const char* hitParts[] = {"Head","Torso"};
            Combo("Hit Part", &s.hitPart, hitParts, 2);
            ImGui::Spacing();
            const char* methods[] = {"Experimental","Legacy"};
            Combo("Method", &s.method, methods, 2);
        });
    }

    // ─────────────────────────────────────────────────────
    //  VISUALS
    // ─────────────────────────────────────────────────────
    void DrawVisuals(FullConfig& cfg) {
        SubTabBar({"ESP","Crosshair","Radar"}, m_visualsSub);
        ImGui::Separator();
        ImGui::Spacing();

        switch (m_visualsSub) {
            case 0: DrawESP(cfg.visuals.esp);           break;
            case 1: DrawCrosshair(cfg.visuals.crosshair);break;
            case 2: DrawRadar(cfg.visuals.radar);       break;
        }
    }

    void DrawESP(ESPCfg& e) {
        TwoCol([&]{
            SectionHeader("General");
            Toggle("Enabled",          &e.enabled);
            Toggle("Team Check",       &e.teamCheck);
            Toggle("Visible Check",    &e.visibleCheck);
            Toggle("Team Based Color", &e.teamBasedColor);
            Toggle("Text Gradient",    &e.textGradient);
            Toggle("Text Background",  &e.textBackground);
            Toggle("Outline",          &e.outline);
            Toggle("Glow",             &e.glow);
            Toggle("Self ESP",         &e.selfESP);
            ImGui::Spacing();
            const char* sizing[] = {"Bounding","Fixed"};
            Combo("Sizing Type", &e.sizingType, sizing, 2);
            SliderF("Render Distance", &e.renderDistance, 100.f, 5000.f, "%.0f");

            ImGui::Spacing();
            SectionHeader("Box");
            Toggle("Box Enabled", &e.boxEnabled);
            Toggle("Fill Box",    &e.fillBox);
            const char* boxTypes[] = {"Corners","Full"};
            Combo("Box Type",   &e.boxType, boxTypes, 2);
            ColorEdit("Enemy Color", &e.boxColorEnemy);
            ColorEdit("Team Color",  &e.boxColorTeam);
            if (e.fillBox) ColorEdit("Fill Color", &e.boxFillColor);

            ImGui::Spacing();
            SectionHeader("Name");
            Toggle("Name Enabled", &e.nameEnabled);
            const char* nameTypes[] = {"Name","Display Name"};
            Combo("Name Type", &e.nameType, nameTypes, 2);
            ColorEdit("Name Color", &e.nameColor);
        }, [&]{
            SectionHeader("Indicators");
            Toggle("Distance",      &e.showDistance);
            Toggle("Equipped Item", &e.showItem);
            Toggle("Skeleton",      &e.showSkeleton);
            Toggle("Head Dot",      &e.headDot);
            Toggle("Head Dot Glow", &e.headDotGlow);
            Toggle("Profile Picture",&e.profilePicture);
            ColorEdit("Skeleton Color", &e.skeletonColor);

            ImGui::Spacing();
            SectionHeader("Health");
            Toggle("Health Bar",   &e.healthBar);
            Toggle("Health Based", &e.healthBased);
            Toggle("Health Text",  &e.healthText);
            const char* textPos[] = {"Above Name","Below Box","Right"};
            Combo("Text Pos", &e.textPos, textPos, 3);
            ColorEdit("Health Bar Color", &e.healthBarColor);

            ImGui::Spacing();
            SectionHeader("Chams");
            Toggle("Chams Enabled", &e.chamsEnabled);
            Toggle("Chams Filled",  &e.chamsFilled);
            const char* chamsMode[] = {"Default","Wireframe","Flat"};
            Combo("Chams Mode",    &e.chamsMode,       chamsMode, 3);
            const char* renderType[] = {"Static","Dynamic"};
            Combo("Render Type",   &e.chamsRenderType, renderType, 2);

            ImGui::Spacing();
            SectionHeader("Tracer");
            Toggle("Tracer", &e.tracerEnabled);
            const char* origin[] = {"Top","Bottom","Middle"};
            Combo("Origin", &e.tracerOrigin, origin, 3);
            ColorEdit("Tracer Color", &e.tracerColor);
        });
    }

    void DrawCrosshair(CrosshairCfg& c) {
        TwoCol([&]{
            SectionHeader("General");
            Toggle("Enabled", &c.enabled);
            Toggle("Dot",     &c.dot);
            ImGui::Spacing();
            const char* styles[] = {"Cross","Circle","Dot","T-Shape"};
            Combo("Style", &c.style, styles, 4);
            SliderF("Size",      &c.size,      1.f, 30.f);
            SliderF("Gap",       &c.gap,       0.f, 20.f);
            SliderF("Thickness", &c.thickness, 0.5f,5.f);
        }, [&]{
            SectionHeader("Color");
            ColorEdit("Color", &c.color);
        });
    }

    void DrawRadar(RadarCfg& r) {
        TwoCol([&]{
            SectionHeader("General");
            Toggle("Enabled", &r.enabled);
            SliderF("Size",   &r.size,  80.f, 300.f, "%.0f");
            SliderF("Range",  &r.range, 50.f, 1000.f,"%.0f");
        }, [&]{
            SectionHeader("Colors");
            ColorEdit("Background",  &r.bgColor);
            ColorEdit("Enemy Color", &r.enemyColor);
            ColorEdit("Team Color",  &r.teamColor);
            ColorEdit("Self Color",  &r.selfColor);
        });
    }

    // ─────────────────────────────────────────────────────
    //  WORLD
    // ─────────────────────────────────────────────────────
    void DrawWorld(FullConfig& cfg) {
        WorldCfg& w = cfg.world;
        TwoCol([&]{
            SectionHeader("Sky");
            Toggle("Custom Sky", &w.customSky);
            if (w.customSky) {
                const char* presets[] = {"Default","Night","Sunset","Custom"};
                Combo("Sky Preset", &w.skyPreset, presets, 4);
                if (w.skyPreset == 3) {
                    ColorEdit("Sky Top",    &w.skyColorTop);
                    ColorEdit("Sky Bottom", &w.skyColorBot);
                }
            }
            ImGui::Spacing();
            SectionHeader("Ambient");
            Toggle("Custom Ambient", &w.customAmbient);
            if (w.customAmbient) {
                ColorEdit("Ambient Color",    &w.ambientColor);
                SliderF("Brightness", &w.ambientBrightness, 0.f, 5.f);
            }
        }, [&]{
            SectionHeader("Lighting");
            Toggle("Custom Lighting", &w.customLighting);
            if (w.customLighting) {
                SliderF("Brightness", &w.brightness, 0.f, 5.f);
                SliderF("Contrast",   &w.contrast,   0.f, 5.f);
                SliderF("Saturation", &w.saturation, 0.f, 5.f);
                SliderF("Gamma",      &w.gamma,      0.1f,3.f);
                ColorEdit("Light Color", &w.lightColor);
            }
            ImGui::Spacing();
            SectionHeader("Camera");
            Toggle("Custom FOV", &w.customFOV);
            if (w.customFOV)
                SliderF("FOV", &w.fov, 20.f, 120.f, "%.0f");
        });
    }

    // ─────────────────────────────────────────────────────
    //  CHARACTER
    // ─────────────────────────────────────────────────────
    void DrawCharacter(FullConfig& cfg) {
        CharacterCfg& ch = cfg.character;
        TwoCol([&]{
            SectionHeader("Hitbox Extender");
            Toggle("Enabled",          &ch.hitboxExtender.enabled);
            Toggle("Visualize Hitbox", &ch.hitboxExtender.visualize);
            Toggle("Team Check",       &ch.hitboxExtender.teamCheck);
            Toggle("Health Check",     &ch.hitboxExtender.healthCheck);
            SliderF("Hitbox Size", &ch.hitboxExtender.hitboxSize, 1.f, 100.f, "%.0f");
            const char* types[] = {"Old","New"};
            Combo("Type", &ch.hitboxExtender.type, types, 2);

            ImGui::Spacing();
            SectionHeader("Target Hovering");
            Toggle("Enabled",       &ch.targetHover.enabled);
            Toggle("Display Circle",&ch.targetHover.displayCircle);
            const char* tm[] = {"Closest To Mouse","Closest To Center"};
            Combo("Target Method",  &ch.targetHover.targetMethod, tm, 2);
            SliderF("Radius", &ch.targetHover.radius, 1.f, 50.f);
            SliderF("Speed",  &ch.targetHover.speed,  0.1f,20.f);
        }, [&]{
            SectionHeader("Movement");
            Toggle("No-Clip",   &ch.movement.noclip);
            Toggle("Inf Jump",  &ch.movement.infJump);
            Toggle("Click TP",  &ch.movement.clickTP);
            ImGui::Spacing();
            Toggle("Speed",     &ch.movement.speedEnabled);
            const char* sm[] = {"Velocity","WalkSpeed"};
            Combo("Speed Method",&ch.movement.speedMethod, sm, 2);
            SliderF("Speed Amount",&ch.movement.speedAmount, 0.1f, 20.f);
            ImGui::Spacing();
            Toggle("Flight",    &ch.movement.flight);
            const char* fm[] = {"Velocity","BodyPosition"};
            Combo("Fly Method", &ch.movement.flyMethod, fm, 2);
            SliderF("Fly Amount",&ch.movement.flyAmount, 0.1f, 20.f);
            ImGui::Spacing();
            Toggle("Jump",      &ch.movement.jumpEnabled);
            SliderF("Jump Power",&ch.movement.jumpPower, 10.f, 500.f, "%.0f");
            ImGui::Spacing();
            Toggle("Float",     &ch.movement.floatEnabled);
            SliderF("Float Height",&ch.movement.floatHeight, 1.f, 100.f);

            ImGui::Spacing();
            SectionHeader("Desync");
            Toggle("Enabled",           &ch.desync.enabled);
            Toggle("Remove Walk Anim",  &ch.desync.removeWalkAnim);
            Toggle("Display Server Pos",&ch.desync.displayServerPos);
            Toggle("Reset on Desync Off",&ch.desync.resetOnOff);
            Toggle("Use Tick",          &ch.desync.useTick);
            SliderI("Desync Interval",  &ch.desync.desyncInterval, 1, 100);
        });
    }

    // ─────────────────────────────────────────────────────
    //  CONFIG
    // ─────────────────────────────────────────────────────
    void DrawConfig() {
        ImGui::Spacing();
        SectionHeader("Saved Configs");

        // List box of configs
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Col::BgWidget);
        ImGui::BeginChild("##cfglist", {300.f, 200.f}, true);
        for (int i = 0; i < (int)g_cfgMgr.configs.size(); i++) {
            bool sel = (g_cfgMgr.activeIdx == i);
            ImGui::PushStyleColor(ImGuiCol_Header,        Col::AccentDim);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Col::TabHov);
            if (ImGui::Selectable(g_cfgMgr.configs[i].name.c_str(), sel))
                g_cfgMgr.activeIdx = i;
            ImGui::PopStyleColor(2);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Spacing();
        // New config name
        ImGui::PushStyleColor(ImGuiCol_FrameBg, Col::BgWidget);
        ImGui::InputText("##newname", m_newCfgName, sizeof(m_newCfgName));
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, Col::Accent);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Col::AccentHov);
        if (ImGui::Button("New")) {
            g_cfgMgr.New(m_newCfgName);
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, {0.5f,0,0,1});
        if (ImGui::Button("Delete") && g_cfgMgr.configs.size() > 1) {
            g_cfgMgr.Delete(g_cfgMgr.activeIdx);
        }
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        SectionHeader("Import / Export");

        ImGui::PushStyleColor(ImGuiCol_FrameBg, Col::BgWidget);
        ImGui::InputText("Path##import", m_importPath, sizeof(m_importPath));
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, Col::Accent);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Col::AccentHov);
        if (ImGui::Button("Import")) {
            g_cfgMgr.Import(m_importPath);
        }
        ImGui::PopStyleColor(2);

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, {0.1f,0.4f,0.1f,1});
        if (ImGui::Button("Export")) {
            std::string path = "ggware_" + g_cfgMgr.Active().name + ".json";
            g_cfgMgr.Export(path, g_cfgMgr.activeIdx);
        }
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, Col::TextDim);
        ImGui::TextWrapped("Configs are saved to %%APPDATA%%\\GgWare\\configs\\");
        ImGui::PopStyleColor();
    }

    // ─────────────────────────────────────────────────────
    //  YOU
    // ─────────────────────────────────────────────────────
    void DrawYou(FullConfig& cfg) {
        YouCfg& y = cfg.you;
        TwoCol([&]{
            SectionHeader("Character Spoofer");
            Toggle("Enabled##spoof", &y.charSpoofer.enabled);
            if (y.charSpoofer.enabled) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, Col::BgWidget);
                ImGui::InputText("Spoof Name", m_spoofName, sizeof(m_spoofName));
                y.charSpoofer.spoofedName = m_spoofName;
                ImGui::InputText("Spoof ID",   m_spoofId,   sizeof(m_spoofId));
                y.charSpoofer.spoofedId   = m_spoofId;
                ImGui::PopStyleColor();
                Toggle("Spoof Team##spoof",  &y.charSpoofer.spoofTeam);
            }
            ImGui::Spacing();
            SectionHeader("Misc");
            Toggle("Anti AFK",        &y.antiAFK);
            Toggle("Rejoin on Kick",  &y.rejoinOnKick);
            Toggle("Custom Cursor",   &y.customCursor);
            Toggle("Hide UI",         &y.hideUI);
            Toggle("Unlock FPS",      &y.unlockFPS);
            if (y.unlockFPS)
                SliderI("Target FPS", &y.targetFPS, 30, 360);
            Toggle("Ping Spoofer",    &y.pingSpoofer);
            if (y.pingSpoofer)
                SliderI("Fake Ping",  &y.fakePing, 1, 999);
        }, [&]{
            SectionHeader("Music Player");
            Toggle("Enabled##music", &y.music.enabled);
            if (y.music.enabled) {
                const char* src[] = {"YouTube","Spotify"};
                Combo("Source", &y.music.source, src, 2);

                ImGui::PushStyleColor(ImGuiCol_FrameBg, Col::BgWidget);
                ImGui::InputText("URL / Playlist", m_songUrl, sizeof(m_songUrl));
                ImGui::PopStyleColor();

                ImGui::Spacing();
                // Playback controls
                ImGui::PushStyleColor(ImGuiCol_Button, Col::Accent);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Col::AccentHov);
                if (ImGui::Button(y.music.isPlaying ? "Pause" : "Play")) {
                    y.music.currentUrl = m_songUrl;
                    y.music.isPlaying  = !y.music.isPlaying;
                    // TODO: hook into audio engine / inject JS bridge
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop")) {
                    y.music.isPlaying = false;
                }
                ImGui::PopStyleColor(2);
                ImGui::SameLine();
                if (ImGui::Button("Add to playlist")) {
                    y.music.playlist.push_back(std::string(m_songUrl));
                }

                SliderF("Volume", &y.music.volume, 0.f, 1.f);
                Toggle("Loop",    &y.music.loop);
                Toggle("Shuffle", &y.music.shuffle);

                ImGui::Spacing();
                // Playlist
                if (!y.music.playlist.empty()) {
                    SectionHeader("Playlist");
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, Col::BgWidget);
                    ImGui::BeginChild("##playlist", {0.f, 120.f}, true);
                    for (int i = 0; i < (int)y.music.playlist.size(); i++) {
                        bool playing = (y.music.currentIndex == i && y.music.isPlaying);
                        if (playing) ImGui::PushStyleColor(ImGuiCol_Text, Col::Accent);
                        char label[64];
                        snprintf(label, sizeof(label), "%s %d. %s",
                            playing ? ">" : " ", i+1,
                            y.music.playlist[i].substr(0,40).c_str());
                        if (ImGui::Selectable(label, playing)) {
                            y.music.currentIndex = i;
                            y.music.currentUrl   = y.music.playlist[i];
                            y.music.isPlaying    = true;
                        }
                        if (playing) ImGui::PopStyleColor();
                    }
                    ImGui::EndChild();
                    ImGui::PopStyleColor();
                }
            }
        });
    }

    // ─────────────────────────────────────────────────────
    //  Helpers
    // ─────────────────────────────────────────────────────

    // Sub-tab bar (horizontal pills)
    void SubTabBar(std::initializer_list<const char*> tabs, int& active) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  {10.f, 4.f});
        int i = 0;
        for (const char* t : tabs) {
            if (i > 0) ImGui::SameLine();
            bool sel = (active == i);
            if (sel) {
                ImGui::PushStyleColor(ImGuiCol_Button,        Col::Accent);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Col::AccentHov);
                ImGui::PushStyleColor(ImGuiCol_Text,          Col::TextBright);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button,        Col::BgWidget);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Col::TabHov);
                ImGui::PushStyleColor(ImGuiCol_Text,          Col::TextNormal);
            }
            if (ImGui::Button(t)) active = i;
            ImGui::PopStyleColor(3);
            i++;
        }
        ImGui::PopStyleVar(2);
    }

    // Two-column layout helper using lambdas
    template<typename L, typename R>
    void TwoCol(L left, R right) {
        float avail = ImGui::GetContentRegionAvail().x;
        float colW  = avail * 0.5f - 6.f;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, {0,0,0,0});

        ImGui::BeginChild("##lc", {colW, 0.f}, false);
        left();
        ImGui::EndChild();

        ImGui::SameLine();

        // Divider line
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(
            p, {p.x, p.y + ImGui::GetContentRegionAvail().y},
            ImGui::ColorConvertFloat4ToU32(Col::Border));
        ImGui::SetCursorScreenPos({p.x + 6.f, p.y});

        ImGui::BeginChild("##rc", {colW, 0.f}, false);
        right();
        ImGui::EndChild();

        ImGui::PopStyleColor();
    }

    // Combo with style
    bool Combo(const char* label, int* v, const char** items, int n) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg,         Col::BgWidget);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  Col::BgHovered);
        ImGui::PushStyleColor(ImGuiCol_PopupBg,         Col::BgPanel);
        ImGui::PushStyleColor(ImGuiCol_Header,          Col::AccentDim);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,   Col::TabHov);
        bool r = ImGui::Combo(label, v, items, n);
        ImGui::PopStyleColor(5);
        return r;
    }
};

} // namespace GgWare

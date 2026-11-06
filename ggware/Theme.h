#pragma once
#include <imgui.h>

// ─────────────────────────────────────────────────────────
//  GgWare Theme  — dark panel, magenta accent
//  Matches the UI in the reference screenshots
// ─────────────────────────────────────────────────────────

namespace GgWare {

    // ── Palette ───────────────────────────────────────────
    namespace Col {
        // Backgrounds
        constexpr ImVec4 BgDeep      = {0.047f, 0.047f, 0.059f, 1.f};  // #0C0C0F
        constexpr ImVec4 BgPanel     = {0.071f, 0.071f, 0.090f, 1.f};  // #121217
        constexpr ImVec4 BgWidget    = {0.094f, 0.094f, 0.118f, 1.f};  // #18181E
        constexpr ImVec4 BgHovered   = {0.118f, 0.118f, 0.149f, 1.f};  // #1E1E26
        constexpr ImVec4 BgActive    = {0.141f, 0.141f, 0.176f, 1.f};  // #24242D

        // Accent — magenta/pink like GgWare
        constexpr ImVec4 Accent      = {0.878f, 0.000f, 0.588f, 1.f};  // #E00096
        constexpr ImVec4 AccentHov   = {1.000f, 0.157f, 0.690f, 1.f};  // #FF28B0
        constexpr ImVec4 AccentDim   = {0.588f, 0.000f, 0.392f, 0.5f}; // dim
        constexpr ImVec4 AccentGlow  = {0.878f, 0.000f, 0.588f, 0.2f}; // glow overlay

        // Text
        constexpr ImVec4 TextBright  = {1.f,    1.f,    1.f,    1.f};
        constexpr ImVec4 TextNormal  = {0.78f,  0.78f,  0.82f,  1.f};
        constexpr ImVec4 TextDim     = {0.45f,  0.45f,  0.50f,  1.f};
        constexpr ImVec4 TextDisabled= {0.30f,  0.30f,  0.34f,  1.f};

        // Borders
        constexpr ImVec4 Border      = {0.15f,  0.15f,  0.20f,  1.f};
        constexpr ImVec4 BorderAccent= {0.878f, 0.000f, 0.588f, 0.6f};

        // Tabs
        constexpr ImVec4 TabActive   = {0.878f, 0.000f, 0.588f, 1.f};
        constexpr ImVec4 TabInactive = {0.094f, 0.094f, 0.118f, 1.f};
        constexpr ImVec4 TabHov      = {0.15f,  0.03f,  0.12f,  1.f};

        // Status colors
        constexpr ImVec4 Green       = {0.18f,  0.90f,  0.35f,  1.f};
        constexpr ImVec4 Red         = {0.95f,  0.18f,  0.18f,  1.f};
        constexpr ImVec4 Yellow      = {0.98f,  0.78f,  0.10f,  1.f};
        constexpr ImVec4 Blue        = {0.20f,  0.55f,  0.98f,  1.f};

        // Slider grab
        constexpr ImVec4 Grab        = {0.878f, 0.000f, 0.588f, 1.f};
        constexpr ImVec4 GrabHov     = {1.000f, 0.157f, 0.690f, 1.f};
    }

    // ── Font sizes ────────────────────────────────────────
    namespace Font {
        constexpr float Small  = 11.f;
        constexpr float Normal = 13.f;
        constexpr float Medium = 15.f;
        constexpr float Large  = 18.f;
        constexpr float Title  = 22.f;
    }

    // ── Window metrics ────────────────────────────────────
    namespace Layout {
        constexpr float WindowW       = 950.f;
        constexpr float WindowH       = 580.f;
        constexpr float SidebarW      = 140.f;
        constexpr float TabBarH       = 36.f;
        constexpr float ChildPad      = 10.f;
        constexpr float ItemSpacingX  = 8.f;
        constexpr float ItemSpacingY  = 6.f;
        constexpr float FrameRounding = 4.f;
        constexpr float GrabRounding  = 4.f;
        constexpr float WindowRounding= 8.f;
        constexpr float ChildRounding = 6.f;
        constexpr float PopupRounding = 6.f;
    }

    inline void ApplyTheme() {
        ImGuiStyle& s = ImGui::GetStyle();

        // ── Rounding ──────────────────────────────────────
        s.WindowRounding    = Layout::WindowRounding;
        s.ChildRounding     = Layout::ChildRounding;
        s.FrameRounding     = Layout::FrameRounding;
        s.GrabRounding      = Layout::GrabRounding;
        s.PopupRounding     = Layout::PopupRounding;
        s.ScrollbarRounding = 4.f;
        s.TabRounding       = 4.f;

        // ── Sizing ────────────────────────────────────────
        s.WindowPadding     = {12.f, 12.f};
        s.FramePadding      = {8.f,  5.f};
        s.ItemSpacing       = {Layout::ItemSpacingX, Layout::ItemSpacingY};
        s.ItemInnerSpacing  = {6.f,  4.f};
        s.ScrollbarSize     = 10.f;
        s.GrabMinSize       = 10.f;
        s.WindowBorderSize  = 1.f;
        s.FrameBorderSize   = 1.f;
        s.ChildBorderSize   = 1.f;
        s.TabBorderSize     = 0.f;
        s.SeparatorTextBorderSize = 1.f;

        // ── Colors ────────────────────────────────────────
        ImVec4* c = s.Colors;

        c[ImGuiCol_WindowBg]            = Col::BgPanel;
        c[ImGuiCol_ChildBg]             = Col::BgWidget;
        c[ImGuiCol_PopupBg]             = Col::BgPanel;
        c[ImGuiCol_Border]              = Col::Border;
        c[ImGuiCol_BorderShadow]        = {0,0,0,0};

        c[ImGuiCol_FrameBg]             = Col::BgWidget;
        c[ImGuiCol_FrameBgHovered]      = Col::BgHovered;
        c[ImGuiCol_FrameBgActive]       = Col::BgActive;

        c[ImGuiCol_TitleBg]             = Col::BgDeep;
        c[ImGuiCol_TitleBgActive]       = Col::BgDeep;
        c[ImGuiCol_TitleBgCollapsed]    = Col::BgDeep;

        c[ImGuiCol_MenuBarBg]           = Col::BgDeep;

        c[ImGuiCol_ScrollbarBg]         = Col::BgDeep;
        c[ImGuiCol_ScrollbarGrab]       = Col::BgActive;
        c[ImGuiCol_ScrollbarGrabHovered]= Col::Accent;
        c[ImGuiCol_ScrollbarGrabActive] = Col::AccentHov;

        c[ImGuiCol_CheckMark]           = Col::Accent;

        c[ImGuiCol_SliderGrab]          = Col::Grab;
        c[ImGuiCol_SliderGrabActive]    = Col::GrabHov;

        c[ImGuiCol_Button]              = Col::BgWidget;
        c[ImGuiCol_ButtonHovered]       = Col::TabHov;
        c[ImGuiCol_ButtonActive]        = Col::AccentDim;

        c[ImGuiCol_Header]              = Col::BgActive;
        c[ImGuiCol_HeaderHovered]       = Col::TabHov;
        c[ImGuiCol_HeaderActive]        = Col::AccentDim;

        c[ImGuiCol_Separator]           = Col::Border;
        c[ImGuiCol_SeparatorHovered]    = Col::Accent;
        c[ImGuiCol_SeparatorActive]     = Col::AccentHov;

        c[ImGuiCol_ResizeGrip]          = Col::BgWidget;
        c[ImGuiCol_ResizeGripHovered]   = Col::Accent;
        c[ImGuiCol_ResizeGripActive]    = Col::AccentHov;

        c[ImGuiCol_Tab]                 = Col::TabInactive;
        c[ImGuiCol_TabHovered]          = Col::TabHov;
        c[ImGuiCol_TabActive]           = Col::TabActive;
        c[ImGuiCol_TabUnfocused]        = Col::TabInactive;
        c[ImGuiCol_TabUnfocusedActive]  = Col::BgActive;

        c[ImGuiCol_Text]                = Col::TextNormal;
        c[ImGuiCol_TextDisabled]        = Col::TextDisabled;

        c[ImGuiCol_DragDropTarget]      = Col::Accent;
        c[ImGuiCol_NavHighlight]        = Col::Accent;

        c[ImGuiCol_PlotLines]           = Col::Accent;
        c[ImGuiCol_PlotLinesHovered]    = Col::AccentHov;
        c[ImGuiCol_PlotHistogram]       = Col::Accent;
        c[ImGuiCol_PlotHistogramHovered]= Col::AccentHov;

        c[ImGuiCol_TableHeaderBg]       = Col::BgDeep;
        c[ImGuiCol_TableBorderStrong]   = Col::Border;
        c[ImGuiCol_TableBorderLight]    = Col::BgActive;
        c[ImGuiCol_TableRowBg]          = {0,0,0,0};
        c[ImGuiCol_TableRowBgAlt]       = {1,1,1,0.03f};

        c[ImGuiCol_ModalWindowDimBg]    = {0,0,0,0.5f};
    }

    // ── Custom Widgets ────────────────────────────────────

    // Toggle (round pill checkbox matching GgWare style)
    inline bool Toggle(const char* label, bool* v) {
        ImGuiWindow* win = ImGui::GetCurrentWindow();
        if (win->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = win->GetID(label);

        const ImVec2 pos    = win->DC.CursorPos;
        const float  h      = ImGui::GetFrameHeight() * 0.9f;
        const float  w      = h * 1.8f;
        const ImRect bb(pos, {pos.x + w + style.ItemInnerSpacing.x + ImGui::CalcTextSize(label).x, pos.y + h});

        ImGui::ItemSize(bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(bb, id)) return false;

        bool hovered, held;
        bool clicked = ImGui::ButtonBehavior(bb, id, &hovered, &held);
        if (clicked) *v = !*v;

        ImDrawList* dl  = win->DrawList;
        float       t   = *v ? 1.f : 0.f;
        // Animate (simple lerp approach via ImGui anim)
        // Track per-id; for simplicity just snap:
        ImVec4 bgCol  = *v ? Col::Accent : Col::BgActive;
        ImU32  bgU32  = ImGui::ColorConvertFloat4ToU32(bgCol);
        dl->AddRectFilled({pos.x, pos.y}, {pos.x+w, pos.y+h}, bgU32, h * 0.5f);
        dl->AddCircleFilled({pos.x + (*v ? w-h*0.5f : h*0.5f), pos.y + h*0.5f}, h*0.4f-1.f, IM_COL32(255,255,255,255));
        if (hovered) dl->AddRect({pos.x,pos.y},{pos.x+w,pos.y+h}, ImGui::ColorConvertFloat4ToU32(Col::AccentHov), h*0.5f, 0, 1.f);

        // Label
        ImVec2 labelPos = {pos.x + w + style.ItemInnerSpacing.x, pos.y + (h - ImGui::GetTextLineHeight()) * 0.5f};
        dl->AddText(labelPos, ImGui::ColorConvertFloat4ToU32(*v ? Col::TextBright : Col::TextNormal), label);

        return clicked;
    }

    // Accent slider — full-width magenta fill
    inline bool SliderF(const char* label, float* v, float mn, float mx, const char* fmt="%.2f") {
        ImGui::PushStyleColor(ImGuiCol_SliderGrab,       Col::Grab);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, Col::GrabHov);
        ImGui::PushStyleColor(ImGuiCol_FrameBg,          Col::BgWidget);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,   Col::BgHovered);
        bool r = ImGui::SliderFloat(label, v, mn, mx, fmt);
        ImGui::PopStyleColor(4);
        return r;
    }

    inline bool SliderI(const char* label, int* v, int mn, int mx) {
        ImGui::PushStyleColor(ImGuiCol_SliderGrab,       Col::Grab);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, Col::GrabHov);
        ImGui::PushStyleColor(ImGuiCol_FrameBg,          Col::BgWidget);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,   Col::BgHovered);
        bool r = ImGui::SliderInt(label, v, mn, mx);
        ImGui::PopStyleColor(4);
        return r;
    }

    // Section header with colored line
    inline void SectionHeader(const char* txt) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        float  w = ImGui::GetContentRegionAvail().x;
        ImGui::GetWindowDrawList()->AddLine(
            {p.x, p.y + ImGui::GetTextLineHeight() * 0.5f + 2},
            {p.x + w, p.y + ImGui::GetTextLineHeight() * 0.5f + 2},
            ImGui::ColorConvertFloat4ToU32(Col::Border));
        ImGui::PushStyleColor(ImGuiCol_Text, Col::Accent);
        ImGui::TextUnformatted(txt);
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    // Color edit button (opens picker)
    inline bool ColorEdit(const char* label, GgWare::Color4* col) {
        float c[4] = {col->r, col->g, col->b, col->a};
        bool r = ImGui::ColorEdit4(label, c, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
        if (r) { col->r=c[0]; col->g=c[1]; col->b=c[2]; col->a=c[3]; }
        return r;
    }

    // Keybind display
    inline void Keybind(const char* label, int* key) {
        char buf[32];
        if (*key == 0) snprintf(buf, sizeof(buf), "None");
        else           snprintf(buf, sizeof(buf), "%c",   (char)*key);
        ImGui::Text("%s", label);
        ImGui::SameLine(140.f);
        ImGui::PushStyleColor(ImGuiCol_Button, Col::BgActive);
        if (ImGui::SmallButton(buf)) { /* TODO: capture next key */ }
        ImGui::PopStyleColor();
    }

} // namespace GgWare

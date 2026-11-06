// ─────────────────────────────────────────────────────────
//  GgWare External — Entry Point
//
//  Engine:   Roblox External (Windows x64)
//  Renderer: DirectX 11 + ImGui 1.90
//  Build:    MSVC 2022 / C++17
//
//  Build steps:
//    1. Clone imgui (https://github.com/ocornut/imgui) into ./imgui/
//    2. Clone nlohmann/json.hpp into ./include/
//    3. Link: d3d11.lib, dxgi.lib, d3dcompiler.lib
//    4. Add imgui backends: imgui_impl_win32.cpp, imgui_impl_dx11.cpp
//    5. Compile all .cpp files together
//    6. Set subsystem to WINDOWS (not Console) for release
//
//  Note on offsets:
//    Pull from your repo (f4n5/Game) and drop them into Offsets.h.
//    Pattern: ReadProcessMemory on RobloxPlayerBeta.exe,
//    resolve pointer chains for DataModel → game → workspace etc.
// ─────────────────────────────────────────────────────────

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <tchar.h>
#include <thread>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

#include "Theme.h"
#include "Config.h"
#include "GUI.h"

// ── Globals ───────────────────────────────────────────────
namespace GgWare {
    ConfigManager g_cfgMgr;
    bool          g_menuOpen  = true;
    GUI           g_gui;
}

static ID3D11Device*           g_pd3dDevice           = nullptr;
static ID3D11DeviceContext*    g_pd3dDeviceContext     = nullptr;
static IDXGISwapChain*         g_pSwapChain            = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView  = nullptr;
static HWND                    g_hWnd                  = nullptr;

// ── D3D helpers ───────────────────────────────────────────
static bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount        = 2;
    sd.BufferDesc.Width   = 0;
    sd.BufferDesc.Height  = 0;
    sd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags              = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow       = hWnd;
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed           = TRUE;
    sd.SwapEffect         = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL fl;
    const D3D_FEATURE_LEVEL fla[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    if (D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            fla, 2, D3D11_SDK_VERSION, &sd,
            &g_pSwapChain, &g_pd3dDevice, &fl, &g_pd3dDeviceContext) != S_OK)
        return false;

    ID3D11Texture2D* pBack = nullptr;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBack));
    g_pd3dDevice->CreateRenderTargetView(pBack, nullptr, &g_mainRenderTargetView);
    pBack->Release();
    return true;
}

static void CleanupDevice() {
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
    if (g_pSwapChain)           { g_pSwapChain->Release();           g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext)    { g_pd3dDeviceContext->Release();     g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice)           { g_pd3dDevice->Release();            g_pd3dDevice = nullptr; }
}

// ── WndProc ───────────────────────────────────────────────
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ── Main ──────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    // Create transparent overlay window
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_CLASSDC;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"GgWareOverlay";
    RegisterClassExW(&wc);

    g_hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
        wc.lpszClassName, L"GgWare",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, hInst, nullptr);

    // Make window transparent (overlay style)
    SetLayeredWindowAttributes(g_hWnd, 0, 255, LWA_ALPHA);

    if (!CreateDeviceD3D(g_hWnd)) {
        CleanupDevice();
        UnregisterClassW(wc.lpszClassName, hInst);
        return 1;
    }

    ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hWnd);

    // ── ImGui init ────────────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename  = nullptr;   // no imgui.ini

    // Font (embedded Segoe UI / fallback to default)
    // To use a custom font: io.Fonts->AddFontFromFileTTF("font.ttf", 13.f);
    io.Fonts->AddFontDefault();

    GgWare::ApplyTheme();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // ── Config init ───────────────────────────────────────
    GgWare::g_cfgMgr.Init();

    // ── Main loop ─────────────────────────────────────────
    MSG msg{};
    while (true) {
        // Poll messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) goto cleanup;
        }

        // Toggle menu on INSERT
        if (GetAsyncKeyState(VK_INSERT) & 1)
            GgWare::g_menuOpen = !GgWare::g_menuOpen;

        // Update click-through based on menu state
        LONG style = GetWindowLong(g_hWnd, GWL_EXSTYLE);
        if (GgWare::g_menuOpen)
            SetWindowLong(g_hWnd, GWL_EXSTYLE, style & ~WS_EX_TRANSPARENT);
        else
            SetWindowLong(g_hWnd, GWL_EXSTYLE, style | WS_EX_TRANSPARENT);

        // Begin frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render GUI
        GgWare::g_gui.Render();

        // Clear + present
        ImGui::Render();
        const float clear[4] = {0, 0, 0, 0};
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);

        // Keep CPU sane
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

cleanup:
    // Auto-save active config on exit
    GgWare::g_cfgMgr.Save(GgWare::g_cfgMgr.Active(),
                           GgWare::g_cfgMgr.Active().name + ".json");

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDevice();
    DestroyWindow(g_hWnd);
    UnregisterClassW(wc.lpszClassName, hInst);
    return 0;
}

#include "renderer.h"

#include "features.h"
#include "version.h"

#include <MinHook.h>
#include <d3d9.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <array>
#include <cstdint>

namespace resamp {

Renderer* Renderer::instance_ = nullptr;

namespace {
using EndSceneFn = HRESULT(WINAPI*)(IDirect3DDevice9*);
using ResetFn = HRESULT(WINAPI*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

EndSceneFn g_original_end_scene{};
ResetFn g_original_reset{};
WNDPROC g_original_wndproc{};
HWND g_game_window{};

BOOL CALLBACK EnumWindowsForProcess(HWND hwnd, LPARAM lparam) {
    DWORD owner_pid{};
    GetWindowThreadProcessId(hwnd, &owner_pid);
    if (owner_pid != GetCurrentProcessId()) return TRUE;
    if (!IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) != nullptr) return TRUE;

    *reinterpret_cast<HWND*>(lparam) = hwnd;
    return FALSE;
}

HWND FindGameWindow() {
    HWND hwnd{};
    EnumWindows(EnumWindowsForProcess, reinterpret_cast<LPARAM>(&hwnd));
    return hwnd;
}

LRESULT CALLBACK HookWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto* renderer = Renderer::Instance();

    if (msg == WM_KEYUP && wparam == VK_F2 && renderer) {
        renderer->ToggleMenu();
        return 0;
    }

    if (renderer && renderer->MenuOpen()) {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) return 1;

        switch (msg) {
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_CHAR:
                return 0;
            default:
                break;
        }
    }

    return CallWindowProcA(g_original_wndproc, hwnd, msg, wparam, lparam);
}

HRESULT WINAPI HookReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    const HRESULT result = g_original_reset(device, params);
    if (SUCCEEDED(result)) ImGui_ImplDX9_CreateDeviceObjects();
    return result;
}

HRESULT WINAPI HookEndScene(IDirect3DDevice9* device) {
    auto* renderer = Renderer::Instance();
    if (!renderer) return g_original_end_scene(device);

    static bool imgui_ready = false;
    if (!imgui_ready) {
        g_game_window = FindGameWindow();
        if (g_game_window) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImGui_ImplWin32_Init(g_game_window);
            ImGui_ImplDX9_Init(device);

            g_original_wndproc = reinterpret_cast<WNDPROC>(
                SetWindowLongPtrA(g_game_window, GWLP_WNDPROC,
                    reinterpret_cast<LONG_PTR>(&HookWndProc))
            );
            imgui_ready = g_original_wndproc != nullptr;
        }
    }

    if (imgui_ready) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (renderer->MenuOpen()) {
            ImGui::SetNextWindowSize(ImVec2(430.0f, 360.0f), ImGuiCond_FirstUseEver);
            bool open = true;
            if (ImGui::Begin(kProjectName, &open, ImGuiWindowFlags_NoCollapse)) {
                ImGui::Text("Version: %s", kVersion);
                ImGui::TextUnformatted("Target: SA-MP 0.3.DL-R1 / x86");
                ImGui::Separator();

                renderer->RenderFeatureMenu();
            }
            ImGui::End();
            if (!open) renderer->ToggleMenu();
        }

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    return g_original_end_scene(device);
}

bool GetD3D9Methods(void** end_scene, void** reset) {
    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProcA;
    wc.hInstance = GetModuleHandleA(nullptr);
    wc.lpszClassName = "resamp_by_cyzenn_dummy";

    if (!RegisterClassExA(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

    HWND window = CreateWindowExA(0, wc.lpszClassName, "", WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);
    if (!window) return false;

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) {
        DestroyWindow(window);
        return false;
    }

    D3DPRESENT_PARAMETERS pp{};
    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = window;

    IDirect3DDevice9* device{};
    const HRESULT hr = d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &pp,
        &device
    );

    if (FAILED(hr) || !device) {
        d3d->Release();
        DestroyWindow(window);
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(device);
    *reset = vtable[16];
    *end_scene = vtable[42];

    device->Release();
    d3d->Release();
    DestroyWindow(window);
    return true;
}

} // namespace

void Renderer::RenderFeatureMenu() {
    features_.RenderMenu();
}

bool Renderer::Install() {
    instance_ = this;

    void* end_scene{};
    void* reset{};
    if (!GetD3D9Methods(&end_scene, &reset)) return false;

    const MH_STATUS init_status = MH_Initialize();
    if (init_status != MH_OK && init_status != MH_ERROR_ALREADY_INITIALIZED) return false;

    if (MH_CreateHook(end_scene, reinterpret_cast<void*>(&HookEndScene),
        reinterpret_cast<void**>(&g_original_end_scene)) != MH_OK) {
        return false;
    }

    if (MH_CreateHook(reset, reinterpret_cast<void*>(&HookReset),
        reinterpret_cast<void**>(&g_original_reset)) != MH_OK) {
        return false;
    }

    if (MH_EnableHook(end_scene) != MH_OK) return false;
    if (MH_EnableHook(reset) != MH_OK) return false;
    return true;
}

} // namespace resamp

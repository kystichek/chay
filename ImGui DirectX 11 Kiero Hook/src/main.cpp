#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>

// Подключение Kiero и ImGui
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

// Подключение наших файлов
#include "globals/globals.hpp"         // Глобальные переменные
#include "visuals/visuals.h"   // ESP и оффсеты
#include "aimBot/aimBbot.h"    // Аимбот (RunAimbot)
#include "includes.h"
#include "aimBot/trigerBot.h"
#include "misc/noFlash.h"
#include "misc/noSmoke.h"
#include "misc/bunnyHop.h"

// Объявление обработчика событий ImGui
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Переменные DirectX и окна
Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

// Переменные меню
bool init = false;
bool draw = false; // Открыто меню или нет
float menu_alpha = 0.0f;
bool menu_fade_in = false;
bool menu_fade_out = false;
int active_tab = 0;

// Цвета
float rainbow_color[3] = { 1.0f, 0.0f, 0.0f };
float rainbow_hue = 0.0f;
ImVec4 accent_color = ImVec4(0.11f, 0.62f, 1.0f, 1.0f);
ImVec4 background_color = ImVec4(0.06f, 0.06f, 0.06f, 0.98f);
ImVec4 text_color = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

void InitImGui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    // 1. Загружаем ОСНОВНОЙ шрифт (текст, русский язык)
    // Убедитесь, что arial.ttf существует
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

    // 2. Настраиваем слияние (Merge) для иконок
    static const ImWchar icon_ranges[] = { 0xE000, 0xF8FF, 0 }; // Диапазон кодов, где лежат иконки
    ImFontConfig config;
    config.MergeMode = true;      // Сливать с предыдущим шрифтом
    config.PixelSnapH = true;     // Выравнивание по пикселям для четкости

    // 3. Загружаем ШРИФТ ИКОНОК
    // ВАЖНО: Путь должен быть верным!
    ImFont* iconFont = io.Fonts->AddFontFromFileTTF("C:\\icons.ttf", 24.0f, &config, icon_ranges);

    // 4. ПРОВЕРКА (ОБЯЗАТЕЛЬНО)
    if (iconFont == NULL) {
        // Если шрифт не найден, выводим ошибку в консоль
        std::cout << "[ERROR] Шрифт C:\\icons.ttf не найден! Иконки работать не будут." << std::endl;

        // Попробуем поискать рядом с exe
        iconFont = io.Fonts->AddFontFromFileTTF("icons.ttf", 24.0f, &config, icon_ranges);
    }

    // Пересобираем атлас шрифтов (важно после мерджа)
    io.Fonts->Build();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);

    // Стили...
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.Colors[ImGuiCol_WindowBg] = background_color;
    style.Colors[ImGuiCol_CheckMark] = accent_color;
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = accent_color;
    style.Colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = accent_color;
    style.Colors[ImGuiCol_Text] = text_color;
}

void UpdateRainbowColor()
{
    rainbow_hue += 0.001f;
    if (rainbow_hue > 1.0f) rainbow_hue = 0.0f;
    float h = rainbow_hue * 6.0f;
    int i = (int)h;
    float f = h - i;
    float q = 1.0f - f;
    float t = f;
    switch (i % 6) {
    case 0: rainbow_color[0] = 1.0f; rainbow_color[1] = t; rainbow_color[2] = 0.0f; break;
    case 1: rainbow_color[0] = q; rainbow_color[1] = 1.0f; rainbow_color[2] = 0.0f; break;
    case 2: rainbow_color[0] = 0.0f; rainbow_color[1] = 1.0f; rainbow_color[2] = t; break;
    case 3: rainbow_color[0] = 0.0f; rainbow_color[1] = q; rainbow_color[2] = 1.0f; break;
    case 4: rainbow_color[0] = t; rainbow_color[1] = 0.0f; rainbow_color[2] = 1.0f; break;
    case 5: rainbow_color[0] = 1.0f; rainbow_color[1] = 0.0f; rainbow_color[2] = q; break;
    }
}

bool FancyCheckbox(const char* label, bool* v)
{
    ImGui::PushStyleColor(ImGuiCol_CheckMark, accent_color);
    bool result = ImGui::Checkbox(label, v);
    ImGui::PopStyleColor(1);
    return result;
}

// --- ХУКИ (HOOKS) ---

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Если меню открыто (draw == true), ImGui перехватывает ввод
    if (draw && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    // Если меню открыто, блокируем ввод в игру (кроме системных клавиш)
    if (draw) {
        // Возвращаем true для сообщений мыши, чтобы не стрелять при клике в меню
        switch (uMsg) {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_MOUSEWHEEL:
            return true;
        }
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }
        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    // Обработка кнопки INSERT для открытия меню
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        draw = !draw;
        if (draw) {
            menu_fade_in = true; menu_fade_out = false; menu_alpha = 0.0f;
        }
        else {
            menu_fade_out = true; menu_fade_in = false;
        }
    }

    // Анимация прозрачности меню
    if (menu_fade_in && menu_alpha < 1.0f) { menu_alpha += 0.05f; if (menu_alpha >= 1.0f) menu_fade_in = false; }
    if (menu_fade_out && menu_alpha > 0.0f) { menu_alpha -= 0.05f; if (menu_alpha <= 0.0f) menu_fade_out = false; }

    UpdateRainbowColor();

    // Начало кадра ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // ---------------------------------------------------------
    // ЛОГИКА ЧИТА (ВЫПОЛНЯЕТСЯ КАЖДЫЙ КАДР)
    // ---------------------------------------------------------

    // 1. Aimbot (Бежит внутри кадра, но не рисует)
    if (globals::aimBot) {
        RunAimbot(visuals::client);
        visuals::drawCircle(globals::aimFov);
    }
    if (globals::triggerBot) {
        RunTriggerBot();
    }

    // 2. Visuals / ESP (Рисует на фоне через GetBackgroundDrawList)
    if (globals::esp) {
        if (globals::espBox) {
            visuals::PlayerBoxESP();
        }
        if (globals::espName) {
            visuals::PlayerNameESP();
        }
        if (globals::espHealth) {
            visuals::PlayerHealthESP();
        }
        if (globals::espWeapon) {
            visuals::PlayerWeaponESP();
        }
        if (globals::espSkeleton) {
            visuals::PlayerSkeletomESP();
        }
    }

    // 3. Misc (Снайперский прицел)
    if (globals::sniper) {
        visuals::sniperHelp();
    }

    if (globals::noFlash) {
        noflash();
    }
    if (globals::noSmoke) {
        noSmoke();
    }
    if (globals::bunnyHop) {
        bunnyHop();
    }

    // ---------------------------------------------------------
    // ОТРИСОВКА МЕНЮ
    // ---------------------------------------------------------

    if (draw || menu_alpha > 0.0f)
    {
        ImGui::SetNextWindowBgAlpha(menu_alpha);
        ImGui::Begin("Elite Menu", &draw, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::SetWindowSize(ImVec2(500, 350));

        ImGui::TextColored(accent_color, "ELITE CHEAT v2.0");
        ImGui::Separator();

        // Вкладки
        ImGui::BeginChild("Tabs", ImVec2(120, 280), true);
        if (ImGui::Button("Visuals", ImVec2(100, 30))) active_tab = 0;
        if (ImGui::Button("Aimbot", ImVec2(100, 30))) active_tab = 1;
        if (ImGui::Button("Misc", ImVec2(100, 30))) active_tab = 2;
        ImGui::EndChild();

        ImGui::SameLine();

        // Контент
        ImGui::BeginChild("Content", ImVec2(340, 280), true);

        if (active_tab == 0) {
            ImGui::Text("Visual Settings");
            FancyCheckbox("Enable ESP (Master Switch)", &globals::esp);
            ImGui::Separator();
            FancyCheckbox("Box ESP", &globals::espBox);
            FancyCheckbox("Name ESP", &globals::espName);
            FancyCheckbox("Health Bar", &globals::espHealth);
            FancyCheckbox("Weapon", &globals::espWeapon);
            FancyCheckbox("Skeleton", &globals::espSkeleton);
        }
        else if (active_tab == 1) {
            ImGui::Text("Aimbot Settings");
            FancyCheckbox("Enable Aimbot", &globals::aimBot);
            ImGui::Separator();
            ImGui::Text("FOV: %.1f", globals::aimFov);
            ImGui::SliderFloat("##fov", &globals::aimFov, 1.0f, 30.0f);


            ImGui::Text("Smooth: %.1f", globals::aimSmooth);
            ImGui::SliderFloat("##smooth", &globals::aimSmooth, 1.0f, 50.0f);

            const char* keys[] = { "Left Ctrl", "Left Alt" };
            ImGui::Text("Key:");
            ImGui::Combo("##key", &globals::aimKey, keys, IM_ARRAYSIZE(keys));

            FancyCheckbox("Enable TrigerBot", &globals::triggerBot);
        }
        else if (active_tab == 2) {
            ImGui::Text("Miscellaneous");
            FancyCheckbox("Sniper Crosshair", &globals::sniper);
            FancyCheckbox("no flash", &globals::noFlash);
            FancyCheckbox("no smoke", &globals::noSmoke);
            FancyCheckbox("bunny hop", &globals::bunnyHop);
        }

        ImGui::EndChild();
        ImGui::End();
    }

    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    bool init_hook = false;
    do
    {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&oPresent, hkPresent);
            init_hook = true;
        }
    } while (!init_hook);
    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        kiero::shutdown();
        break;
    }
    return TRUE;
}
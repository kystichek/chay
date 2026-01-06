#include "visuals.h"
#include "../math/math.h"
#include "../imgui/imgui.h"
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <map>
#include <sstream> 

// 1. Инициализация переменных (Обязательно!)
uintptr_t visuals::client = reinterpret_cast<uintptr_t>(GetModuleHandle("client.dll"));
uintptr_t visuals::engine2 = reinterpret_cast<uintptr_t>(GetModuleHandle("engine2.dll"));

#define WEAPON_KNIFE 1
#define WEAPON_PISTOL 2
#define WEAPON_SHOTGUN 3
#define WEAPON_SUBMACHINE 4
#define WEAPON_RIFELS 5
#define WEAPON_SNIPER 6
#define WEAPON_MACHINE 7
#define WEAPON_GRENADE 8
#define WEAPON_BOMB 9
#define WEAPON_REVOLVER 10
#define WEAPON_CZ75A 11
#define WEAPON_TASER 12
#define WEAPON_AUTOSNIPER 13



// 2. Вспомогательные функции (Entity List)
uintptr_t visuals::GetEntityById(int i) {
    if (!client) return 0;
    uintptr_t entityList = *(uintptr_t*)(client + offsets::dwEntityList);
    if (!entityList) return 0;
    uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
    if (!listEntry) return 0;
    uintptr_t playerController = *(uintptr_t*)(listEntry + 112 * (i & 0x1FF));
    if (!playerController) return 0;
    uint32_t playerPawn = *(uint32_t*)(playerController + offsets::m_hPlayerPawn);
    if (!playerPawn) return 0;
    uintptr_t listEntry2 = *(uintptr_t*)(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
    if (!listEntry2) return 0;
    return *(uintptr_t*)(listEntry2 + 112 * (playerPawn & 0x1FF));
}


uintptr_t visuals::GetPlayerController(int i) {
    if (!client) return 0;
    uintptr_t entityList = *(uintptr_t*)(client + offsets::dwEntityList);
    if (!entityList) return 0;
    uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
    if (!listEntry) return 0;
    return *(uintptr_t*)(listEntry + 112 * (i & 0x1FF));
}

void visuals::drawCircle(float fov)
{
    auto draw = ImGui::GetBackgroundDrawList();
    if (!engine2) return;
    uint32_t screenWidth = *(uint32_t*)(engine2 + offsets::dwWindowWidth);
    uint32_t screenHeight = *(uint32_t*)(engine2 + offsets::dwWindowHeight);

    float center_x = screenWidth / 2.0f;
    float center_y = screenHeight / 2.0f;

    draw->AddCircle({ center_x, center_y }, fov * 7.6, ImColor(255, 255, 255));
}


// 3. Рисование текста
void Texts(std::string Text, Vec2 Pos, ImColor Color, float FontSize, bool KeepCenter) {
    if (!ImGui::GetFont()) return;
    float TextWidth = ImGui::GetFont()->CalcTextSizeA(FontSize, FLT_MAX, 0.f, Text.c_str()).x;
    ImVec2 Pos_ = { Pos.x - TextWidth / 2, Pos.y };
    ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), FontSize, Pos_, Color, Text.c_str());
}

void StrokeText(std::string Text, Vec2 Pos, ImColor Color, float FontSize, bool KeepCenter) {
    Texts(Text, { Pos.x - 1, Pos.y + 1 }, ImColor(0, 0, 0), FontSize, KeepCenter);
    Texts(Text, { Pos.x - 1, Pos.y - 1 }, ImColor(0, 0, 0), FontSize, KeepCenter);
    Texts(Text, { Pos.x + 1, Pos.y + 1 }, ImColor(0, 0, 0), FontSize, KeepCenter);
    Texts(Text, { Pos.x + 1, Pos.y - 1 }, ImColor(0, 0, 0), FontSize, KeepCenter);
    Texts(Text, Pos, Color, FontSize, KeepCenter);
}


// 4. Реализация ESP функций

void visuals::PlayerBoxESP()
{
    float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + offsets::dwViewMatrix);
    auto localPlayerPawn = *(uintptr_t*)(client + offsets::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;
    auto localTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);

    for (int i = 1; i < 64; i++) {
        uintptr_t PlayerPawn = GetEntityById(i);
        if (!PlayerPawn) continue;

        int health = *(int*)(PlayerPawn + offsets::m_iHealth);
        if (health <= 0 || health > 100) continue;
        int team = *(int*)(PlayerPawn + offsets::m_iTeamNum);
        if (team == localTeam) continue;

        uintptr_t GameSceneNode = *(uintptr_t*)(PlayerPawn + offsets::m_pGameSceneNode);
        if (!GameSceneNode) continue;
        uintptr_t BoneArray = *(uintptr_t*)(GameSceneNode + offsets::m_modelState + 0x80);
        if (!BoneArray) continue;

        Vec3 feetPos = *(Vec3*)(PlayerPawn + offsets::m_vOldOrigin);
        Vec3 headPos = *(Vec3*)(BoneArray + bones::head * 32);

        Vec2 feet, head;
        float depth;
        if (feetPos.WorldToScreen(feet, ViewMatrix, depth) && headPos.WorldToScreen(head, ViewMatrix, depth)) {
            float entityHeight = feet.y - head.y;
            float boxHeight = entityHeight * 1.2f;
            float boxWidth = boxHeight / 2.0f;
            float left = feet.x - (boxWidth / 2);
            float right = feet.x + (boxWidth / 2);
            float bottom = feet.y;
            float top = feet.y - boxHeight;

            ImGui::GetBackgroundDrawList()->AddRect({ left, top }, { right, bottom }, ImColor(255, 0, 0), 2.0f);
            
        }
    }
}

void visuals::PlayerSkeletomESP()
{
    float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + offsets::dwViewMatrix);
    auto localPlayerPawn = *(uintptr_t*)(client + offsets::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;
    auto localTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);

    for (int i = 1; i < 64; i++) {
        uintptr_t PlayerPawn = GetEntityById(i);
        if (!PlayerPawn) continue;
        int health = *(int*)(PlayerPawn + offsets::m_iHealth);
        if (health <= 0) continue;
        int team = *(int*)(PlayerPawn + offsets::m_iTeamNum);
        if (team == localTeam) continue;

        uintptr_t GameSceneNode = *(uintptr_t*)(PlayerPawn + offsets::m_pGameSceneNode);
        if (!GameSceneNode) continue;
        uintptr_t BoneArray = *(uintptr_t*)(GameSceneNode + offsets::m_modelState + 0x80);
        if (!BoneArray) continue;

        // Кости
        Vec3 headPos = *(Vec3*)(BoneArray + bones::head * 32);
        Vec3 neckPos = *(Vec3*)(BoneArray + bones::neck * 32);
        Vec3 spinePos = *(Vec3*)(BoneArray + bones::spine * 32);
        Vec3 spine1Pos = *(Vec3*)(BoneArray + bones::spine_1 * 32);
        Vec3 cockPos = *(Vec3*)(BoneArray + bones::cock * 32);
        Vec3 l_shoulderPos = *(Vec3*)(BoneArray + bones::left_shoulder * 32);
        Vec3 l_armPos = *(Vec3*)(BoneArray + bones::left_arm * 32);
        Vec3 l_handPos = *(Vec3*)(BoneArray + bones::left_hand * 32);
        Vec3 r_shoulderPos = *(Vec3*)(BoneArray + bones::right_shoulder * 32);
        Vec3 r_armPos = *(Vec3*)(BoneArray + bones::right_arm * 32);
        Vec3 r_handPos = *(Vec3*)(BoneArray + bones::right_hand * 32);
        Vec3 l_hipPos = *(Vec3*)(BoneArray + bones::left_hip * 32);
        Vec3 l_kneePos = *(Vec3*)(BoneArray + bones::left_knee * 32);
        Vec3 l_feetPos = *(Vec3*)(BoneArray + bones::left_feet * 32);
        Vec3 r_hipPos = *(Vec3*)(BoneArray + bones::right_hip * 32);
        Vec3 r_kneePos = *(Vec3*)(BoneArray + bones::right_knee * 32);
        Vec3 r_feetPos = *(Vec3*)(BoneArray + bones::right_feet * 32);

        float depth;
        auto draw = ImGui::GetBackgroundDrawList();

        auto DrawBoneLine = [&](Vec3& start, Vec3& end) {
            Vec2 s1, s2;
            if (start.WorldToScreen(s1, ViewMatrix, depth) && end.WorldToScreen(s2, ViewMatrix, depth)) {
                draw->AddLine(ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y), ImColor(0, 255, 0), 1.0f);
            }
            };

        DrawBoneLine(headPos, neckPos);
        DrawBoneLine(neckPos, spinePos);
        DrawBoneLine(spinePos, spine1Pos);
        DrawBoneLine(spine1Pos, cockPos);
        DrawBoneLine(neckPos, l_shoulderPos);
        DrawBoneLine(l_shoulderPos, l_armPos);
        DrawBoneLine(l_armPos, l_handPos);
        DrawBoneLine(neckPos, r_shoulderPos);
        DrawBoneLine(r_shoulderPos, r_armPos);
        DrawBoneLine(r_armPos, r_handPos);
        DrawBoneLine(cockPos, l_hipPos);
        DrawBoneLine(l_hipPos, l_kneePos);
        DrawBoneLine(l_kneePos, l_feetPos);
        DrawBoneLine(cockPos, r_hipPos);
        DrawBoneLine(r_hipPos, r_kneePos);
        DrawBoneLine(r_kneePos, r_feetPos);
    }
}

void visuals::PlayerNameESP()
{
    float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + offsets::dwViewMatrix);
    auto localPlayerPawn = *(uintptr_t*)(client + offsets::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;
    auto localTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);

    for (int i = 1; i < 64; i++) {
        uintptr_t PlayerPawn = GetEntityById(i);
        uintptr_t playerController = GetPlayerController(i);
        if (!PlayerPawn || !playerController) continue;

        int health = *(int*)(PlayerPawn + offsets::m_iHealth);
        if (health <= 0) continue;
        int team = *(int*)(PlayerPawn + offsets::m_iTeamNum);
        if (team == localTeam) continue;

        uintptr_t GameSceneNode = *(uintptr_t*)(PlayerPawn + offsets::m_pGameSceneNode);
        if (!GameSceneNode) continue;
        uintptr_t BoneArray = *(uintptr_t*)(GameSceneNode + offsets::m_modelState + 0x80);
        if (!BoneArray) continue;

        Vec3 feetPos = *(Vec3*)(PlayerPawn + offsets::m_vOldOrigin);
        Vec3 headPos = *(Vec3*)(BoneArray + bones::head * 32);

        Vec2 feet, head;
        float depth;
        if (feetPos.WorldToScreen(feet, ViewMatrix, depth) && headPos.WorldToScreen(head, ViewMatrix, depth)) {
            char* ptr = reinterpret_cast<char*>(playerController + offsets::m_iszPlayerName);
            std::string result;
            if (ptr) {
                // Безопасное чтение имени (до 32 символов)
                for (size_t k = 0; k < 32; ++k) {
                    char c = *(ptr + k);
                    if (c == '\0') break;
                    result += c;
                }
            }
            if (result.empty()) result = "Enemy";

            float entityHeight = feet.y - head.y;
            float boxHeight = entityHeight * 1.2f;
            float top = feet.y - boxHeight;

            StrokeText(result, { feet.x, top - 15 }, ImColor(255, 255, 255), 14, true);
        }
    }
}

void visuals::PlayerHealthESP()
{
    float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + offsets::dwViewMatrix);
    auto localPlayerPawn = *(uintptr_t*)(client + offsets::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;
    auto localTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);

    for (int i = 1; i < 64; i++) {
        uintptr_t PlayerPawn = GetEntityById(i);
        if (!PlayerPawn) continue;
        int health = *(int*)(PlayerPawn + offsets::m_iHealth);
        if (health <= 0) continue;
        int team = *(int*)(PlayerPawn + offsets::m_iTeamNum);
        if (team == localTeam) continue;

        uintptr_t GameSceneNode = *(uintptr_t*)(PlayerPawn + offsets::m_pGameSceneNode);
        if (!GameSceneNode) continue;
        uintptr_t BoneArray = *(uintptr_t*)(GameSceneNode + offsets::m_modelState + 0x80);
        if (!BoneArray) continue;

        Vec3 feetPos = *(Vec3*)(PlayerPawn + offsets::m_vOldOrigin);
        Vec3 headPos = *(Vec3*)(BoneArray + bones::head * 32);

        Vec2 feet, head;
        float depth;
        if (feetPos.WorldToScreen(feet, ViewMatrix, depth) && headPos.WorldToScreen(head, ViewMatrix, depth)) {
            auto draw = ImGui::GetBackgroundDrawList();
            float height = (feet.y - head.y) * 1.2f;
            float width = height / 2.0f;
            float x = feet.x - width / 2;

            float barWidth = 4.0f;
            float barOffset = 5.0f;
            float barLeft = x - barOffset - barWidth;
            float barRight = x - barOffset;
            float barBottom = feet.y;
            float barTop = feet.y - height;

            draw->AddRectFilled({ barLeft, barTop }, { barRight, barBottom }, ImColor(50, 50, 50));
            float healthPercent = (std::max)(0.0f, (std::min)(1.0f, (float)health / 100.0f));
            float greenHeight = height * healthPercent;
            draw->AddRectFilled({ barLeft, barBottom - greenHeight }, { barRight, barBottom }, ImColor(0, 255, 0));
            draw->AddRect({ barLeft, barTop }, { barRight, barBottom }, ImColor(0, 0, 0), 0.0f, 0, 1.0f);
        }
    }
}



void visuals::PlayerWeaponESP()
{

}

void visuals::sniperHelp()
{
    auto draw = ImGui::GetBackgroundDrawList();
    auto localPlayerPawn = *(uintptr_t*)(client + offsets::dwLocalPlayerPawn);
    if (!engine2) return;
    uint32_t screenWidth = *(uint32_t*)(engine2 + offsets::dwWindowWidth);
    uint32_t screenHeight = *(uint32_t*)(engine2 + offsets::dwWindowHeight);

    float center_x = screenWidth / 2.0f;
    float center_y = screenHeight / 2.0f;

    draw->AddCircle({ center_x, center_y }, 4.0f, ImColor(255, 0, 0));


    int flags = *(int*)(localPlayerPawn +offsets::m_fFlags);
    StrokeText(std::to_string(flags), { 100, 100 }, ImColor(0, 255, 0), 20.0f, false);
}
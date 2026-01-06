#include "aimBbot.h"
#include "../globals/globals.hpp"
#include "../math/math.h"
#include "../visuals/visuals.h"
#include <Windows.h>

void RunAimbot(uintptr_t clientBase) {
    // Проверка кнопки из настроек
    int key = (globals::aimKey == 0) ? VK_CONTROL : VK_MENU; // Ctrl или Alt
    if (!GetAsyncKeyState(key)) return;

    // Ваши настройки из меню
    float aimFov = globals::aimFov;
    float smooth = globals::aimSmooth;

    // Получение локального игрока
    auto localPlayerPawn = *(uintptr_t*)(clientBase + offsets::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;
    auto localTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);

    // Координаты головы
    Vec3 localPos = *(Vec3*)(localPlayerPawn + offsets::m_vOldOrigin);
    Vec3 viewOffset = *(Vec3*)(localPlayerPawn + offsets::m_vecViewOffset);
    Vec3 localHead = { localPos.x + viewOffset.x, localPos.y + viewOffset.y, localPos.z + viewOffset.z };

    Vec3* viewAnglesPtr = (Vec3*)(clientBase + offsets::dwViewAngles);
    Vec3 currentAngles = *viewAnglesPtr;

    float bestFov = aimFov;
    Vec3 bestAngle = { 0, 0, 0 };

    for (int i = 1; i < 64; i++) {
        uintptr_t entity = visuals::GetEntityById(i);
        if (!entity || entity == localPlayerPawn) continue;

        int health = *(int*)(entity + offsets::m_iHealth);
        int team = *(int*)(entity + offsets::m_iTeamNum);
        if (health <= 0 || team == localTeam) continue;

        // Получение костей
        uintptr_t gameSceneNode = *(uintptr_t*)(entity + offsets::m_pGameSceneNode);
        if (!gameSceneNode) continue;
        uintptr_t boneArray = *(uintptr_t*)(gameSceneNode + 0x190 + 0x80); // Проверьте оффсеты!
        if (!boneArray) continue;

        Vec3 enemyHead = *(Vec3*)(boneArray + 6 * 32);

        // Расчет
        Vec3 angleToEnemy = CalculateAngle(localHead, enemyHead);
        Vec3 delta = { angleToEnemy.x - currentAngles.x, angleToEnemy.y - currentAngles.y, 0 };
        NormalizeAngles(delta);

        float fov = sqrt(delta.x * delta.x + delta.y * delta.y);

        if (fov < bestFov) {
            bestFov = fov;
            bestAngle = angleToEnemy;
        }
    }

    if (bestFov < aimFov) {
        Vec3 delta = { bestAngle.x - currentAngles.x, bestAngle.y - currentAngles.y, 0 };
        NormalizeAngles(delta);

        // Применяем Smooth
        Vec3 smoothedAngle;
        smoothedAngle.x = currentAngles.x + delta.x / smooth;
        smoothedAngle.y = currentAngles.y + delta.y / smooth;
        NormalizeAngles(smoothedAngle);

        *viewAnglesPtr = smoothedAngle;
    }
}
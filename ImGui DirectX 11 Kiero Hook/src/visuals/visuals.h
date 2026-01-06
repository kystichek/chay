#pragma once
#include <cstdint>
#include <cstddef>

namespace offsets {
    constexpr std::ptrdiff_t dwEntityList = 0x1D13CE8; // ПРОВЕРЬТЕ АКТУАЛЬНОСТЬ!
    constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1BEEF28;
    constexpr std::ptrdiff_t dwViewMatrix = 0x1E323D0;
    constexpr std::ptrdiff_t dwWindowHeight = 0x8EF844;
    constexpr std::ptrdiff_t dwWindowWidth = 0x8EF840;
    constexpr std::ptrdiff_t m_hPlayerPawn = 0x8FC;
    constexpr std::ptrdiff_t m_iTeamNum = 0x3EB;
    constexpr std::ptrdiff_t m_vOldOrigin = 0x15A0;
    constexpr std::ptrdiff_t m_iHealth = 0x34C;
    constexpr std::ptrdiff_t m_iszPlayerName = 0x6E8;
    constexpr std::ptrdiff_t m_pGameSceneNode = 0x330;
    constexpr std::ptrdiff_t m_modelState = 0x190;
    constexpr std::ptrdiff_t m_vecViewOffset = 0xD80;
    constexpr std::ptrdiff_t dwViewAngles = 0x1E3C800;
    constexpr std::ptrdiff_t m_flFlashDuration = 0x1610;
    constexpr std::ptrdiff_t m_iIDEntIndex = 0x3ECC;
    constexpr std::ptrdiff_t m_fFlags = 0x3F8;
    constexpr std::ptrdiff_t dwForceJump = 0x1BE88B0;
}

enum bones : int {
    head = 6, neck = 5, spine = 4, spine_1 = 2,
    left_shoulder = 8, left_arm = 9, left_hand = 11,
    cock = 0,
    right_shoulder = 13, right_arm = 14, right_hand = 16,
    left_hip = 22, left_knee = 23, left_feet = 24,
    right_hip = 25, right_knee = 26, right_feet = 27
};

namespace visuals {
    extern uintptr_t client;
    extern uintptr_t engine2;

    void PlayerBoxESP();
    void PlayerSkeletomESP();
    void PlayerNameESP();
    void PlayerHealthESP();
    void PlayerWeaponESP();
    void sniperHelp();

    // Вспомогательные функции для получения сущностей
    uintptr_t GetEntityById(int i);
    uintptr_t GetPlayerController(int i);

    void drawCircle(float fov);
}
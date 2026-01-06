#pragma once

struct globals
{
    static bool esp;
    static bool espBox;
    static bool espName;
    static bool espHealth;
    static bool espSkeleton;
    static bool espWeapon;
    static bool sniper;
    static bool aimBot;
    static bool triggerBot;
    static bool noFlash;
    static bool noSmoke;
    static bool bunnyHop;

    // Настройки аима
    static float aimFov;
    static float aimSmooth;
    static int aimKey; // 0 = Ctrl, 1 = Alt
};
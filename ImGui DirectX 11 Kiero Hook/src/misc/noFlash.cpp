#include "noFlash.h"
#include <cstdint>

void noflash()
{
    uintptr_t localPlayer = *(uintptr_t*)(visuals::client + offsets::dwLocalPlayerPawn);
    if (!localPlayer) return;

        float flashDuration = *(float*)(localPlayer + offsets::m_flFlashDuration);
        if (flashDuration > 0.0f) {
            *(float*)(localPlayer + offsets::m_flFlashDuration) = 0.0f;
    }
}

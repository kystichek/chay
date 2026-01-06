#include "math.h"
#include "../visuals/visuals.h" // Теперь здесь можно безопасно подключить visuals
#include <cstdint>

// Реализация функции
bool Vec3::WorldToScreen(Vec2& out, float(*ViewMatrix)[4][4], float& depth) const {
    // Формула W-компонента
    float w = (*ViewMatrix)[3][0] * x + (*ViewMatrix)[3][1] * y + (*ViewMatrix)[3][2] * z + (*ViewMatrix)[3][3];

    // Если объект сзади нас (w < 0.01), возвращаем false
    if (w < 0.01f) {
        return false;
    }

    // Записываем дистанцию
    depth = w;

    float invW = 1.0f / w;

    // Безопасно получаем размеры экрана
    // Если engine2 еще не найден, берем дефолтные значения, чтобы не крашнуло
    uint32_t screenWidth = 1920;
    uint32_t screenHeight = 1080;

    if (visuals::engine2) {
        screenWidth = *(uint32_t*)(visuals::engine2 + offsets::dwWindowWidth);
        screenHeight = *(uint32_t*)(visuals::engine2 + offsets::dwWindowHeight);
    }

    float x_ = (*ViewMatrix)[0][0] * x + (*ViewMatrix)[0][1] * y + (*ViewMatrix)[0][2] * z + (*ViewMatrix)[0][3];
    float y_ = (*ViewMatrix)[1][0] * x + (*ViewMatrix)[1][1] * y + (*ViewMatrix)[1][2] * z + (*ViewMatrix)[1][3];

    // Используем float деление (2.0f), чтобы было точнее
    float halfW = (float)screenWidth / 2.0f;
    float halfH = (float)screenHeight / 2.0f;

    out.x = halfW + (x_ * invW) * halfW;
    out.y = halfH - (y_ * invW) * halfH;

    return true;
}
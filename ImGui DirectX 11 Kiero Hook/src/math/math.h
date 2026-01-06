#pragma once
#include <cmath>

#define M_PI 3.14159265358f

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;

    // Конструктор
    constexpr Vec3(float x = 0.f, float y = 0.f, float z = 0.f) : x(x), y(y), z(z) {}

    // ТОЛЬКО ОБЪЯВЛЕНИЕ (код убираем в math.cpp)
    // const в конце означает, что функция не меняет сам Vec3
    bool WorldToScreen(Vec2& out, float(*ViewMatrix)[4][4], float& depth) const;
};

// Эти функции маленькие, их оставляем inline в заголовке
inline Vec3 CalculateAngle(const Vec3& localPos, const Vec3& enemyPos) {
    Vec3 delta;
    delta.x = enemyPos.x - localPos.x;
    delta.y = enemyPos.y - localPos.y;
    delta.z = enemyPos.z - localPos.z;

    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

    Vec3 angles;
    angles.x = -atan2(delta.z, hyp) * (180.0f / M_PI);
    angles.y = atan2(delta.y, delta.x) * (180.0f / M_PI);
    angles.z = 0.0f;

    return angles;
}

inline void NormalizeAngles(Vec3& angle) {
    while (angle.x > 89.0f) angle.x = 89.0f;
    while (angle.x < -89.0f) angle.x = -89.0f;
    while (angle.y > 180.0f) angle.y -= 360.0f;
    while (angle.y < -180.0f) angle.y += 360.0f;
    angle.z = 0.0f;
}
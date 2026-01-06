#include "noSmoke.h"
#include <string>


void noSmoke()
{
    // Проверка на включенную функцию и клиент
    if (!visuals::client) return;

    uintptr_t entityList = *(uintptr_t*)(visuals::client + offsets::dwEntityList);
    if (!entityList) return;

    // Цикл по всем сущностям (Гранаты обычно находятся после 64-го индекса)
    // Перебираем до 1024 (максимум сущностей)
    for (int i = 64; i < 1024; i++) {

        // 1. Получаем сущность (стандартный алгоритм списка)
        uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * ((i & 0x7FFF) >> 9)) + 16);
        if (!listEntry) continue;

        // ВАЖНО: 120 (0x78) - размер записи
        uintptr_t entity = *(uintptr_t*)(listEntry + 112 * (i & 0x1FF));
        if (!entity) continue;

        // 2. Получаем ИМЯ сущности (чтобы понять, что это дым)
        // Entity Identity лежит по смещению 0x10
        uintptr_t entityIdentity = *(uintptr_t*)(entity + 0x10);
        if (!entityIdentity) continue;

        // Designer Name (строка) лежит по смещению 0x20 внутри Identity
        uintptr_t designerNamePtr = *(uintptr_t*)(entityIdentity + 0x20);
        if (!designerNamePtr) continue;

        // Читаем имя (первые 20 символов достаточно)
        char nameBuffer[32];
        // В Internal чите можно использовать memcpy или reinterpret_cast, если память доступна
        // Но лучше читать как строку
        char* val = (char*)designerNamePtr;

        // Проверка на nullptr перед чтением строки!
        if (!val) continue;

        std::string name(val);

        // 3. Если это ДЫМ
        if (name.find("smokegrenade") != std::string::npos) {

            // Способ 1: Телепортация под землю (Самый надежный)
            // Берем оффсет координат (m_vOldOrigin = 0x1274 или 0x1224, проверь visuals.h)
            // Обычно он совпадает с координатами игрока

            // Читаем текущую Z координату
            float* zPos = (float*)(entity + offsets::m_vOldOrigin + 8); // +8 это Z (x=0, y=4, z=8)

            // Отправляем дым в ад (под карту)
            *zPos = -20000.0f;

            // Способ 2: Отключение тиков (если телепорт не нравится)
            // *(int*)(entity + 0x1160) = 0; // m_nSmokeEffectTickBegin
        }
    }
}

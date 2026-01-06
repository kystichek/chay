#include <Windows.h>
#include <cstdint>
#include <thread>
#include "trigerBot.h"

// Твои оффсеты (предполагаем, что они верные и находятся в пространстве имен offsets)
// Важно: client должен быть базовым адресом модуля client.dll
uintptr_t clientBase = (uintptr_t)GetModuleHandle("client.dll");

// Хелпер-функция для получения адреса игрока (Pawn) по его ID (Index)
// Это стандартная математика для EntityList в CS2
uintptr_t GetPawnByIndex(int index)
{
    // 1. Получаем адрес списка сущностей
    uintptr_t entityList = *(uintptr_t*)(clientBase + offsets::dwEntityList);
    if (!entityList) return 0;

    // 2. Находим нужную запись (List Entry)
    // CS2 использует разделение страниц: index >> 9
    uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * (index >> 9) + 16));
    if (!listEntry) return 0;

    // 3. Достаем саму пешку (Pawn)
    // index & 0x1FF - это маска для получения индекса внутри страницы
    uintptr_t pawn = *(uintptr_t*)(listEntry + 112 * (index & 0x1FF));

    return pawn;
}

void SimulateLeftClick()
{
    // Для Internal чита лучше хукать CreateMove, но mouse_event тоже сойдет для начала
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    // Для Internal задержка может быть меньше, так как работаем в том же процессе
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void RunTriggerBot()
{
    // 1. Получаем адрес локального игрока
    uintptr_t localPlayerPawn = *(uintptr_t*)(clientBase + offsets::dwLocalPlayerPawn);
    if (!localPlayerPawn) return;

    // 2. Узнаем ID сущности под прицелом (m_iIDEntIndex)
    // В CS2 это поле обновляется игрой, когда прицел на враге
    int crosshairID = *(int*)(localPlayerPawn + offsets::m_iIDEntIndex);

    // Если ID <= 0, значит мы ни на кого не смотрим (или смотрим в стену)
    if (crosshairID <= 0) return;

    // 3. Получаем адрес того, на кого смотрим
    uintptr_t targetPawn = GetPawnByIndex(crosshairID);
    if (!targetPawn) return; // Если не нашли (например, объект уже удален)

    // Проверка: не смотрим ли мы сами на себя (баг спектатора)
    if (targetPawn == localPlayerPawn) return;

    // 4. Читаем данные для проверки "Свой-Чужой" и "Жив-Мертв"
    int myTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);
    int targetTeam = *(int*)(targetPawn + offsets::m_iTeamNum);
    int targetHealth = *(int*)(targetPawn + offsets::m_iHealth);

    // 5. Логика стрельбы
    if (targetHealth > 0 && myTeam != targetTeam)
    {
        // ОГОНЬ!
        SimulateLeftClick();

        // Анти-спам задержка (чтобы не стрелять 100 раз в секунду в труп или при отдаче)
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}
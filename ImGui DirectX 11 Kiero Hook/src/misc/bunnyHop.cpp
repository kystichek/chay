#include "bunnyHop.h"

void bunnyHop()
{

    auto localPlayer = *reinterpret_cast<uintptr_t*>(visuals::client + offsets::dwLocalPlayerPawn);

    // Если игрока нет (мы в меню или не заспавнились) - выходим
    if (!localPlayer) return;

    // 3. Чтение данных
    int flags = *reinterpret_cast<int*>(localPlayer + offsets::m_fFlags);

    // 4. Логика распрыжки
    // Проверяем, зажат ли ПРОБЕЛ (асинхронно, работает даже если окно не активно, но для интернала это ок)
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        // Получаем адрес переменной прыжка в памяти
        auto forceJumpAddr = reinterpret_cast<int*>(visuals::client + offsets::dwForceJump);

        // Проверяем бит "На земле" (побитовое И)
        // Это надежнее, чем сравнивать с числом 65665
        if (flags & FL_ONGROUND)
        {
            // Мы касаемся земли -> ОТПРАВЛЯЕМ ПРЫЖОК
            *forceJumpAddr = PLUS_JUMP;
        }
        else
        {
            // Мы в воздухе -> СБРАСЫВАЕМ ПРЫЖОК
            // Это критически важно! Если не отправить MINUS_JUMP,
            // игра будет думать, что пробел зажат физически, и не даст прыгнуть снова при приземлении.
            *forceJumpAddr = MINUS_JUMP;
        }
    }
    
}

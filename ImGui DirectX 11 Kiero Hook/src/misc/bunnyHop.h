#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <thread>

#include "../visuals/visuals.h"

constexpr int FL_ONGROUND = (1 << 0);
constexpr int PLUS_JUMP = 65537; // Нажать (+jump)
constexpr int MINUS_JUMP = 256;  // Отпустить (-jump)

void bunnyHop();


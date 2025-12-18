#define NOMINMAX
#include "visuals.h"
#include <cstddef>
#include <cstdint>
#include "../math/math.h"
#include <thread>
#include <chrono>
#include "../imgui/imgui.h"
#include <algorithm>
#include <iostream>

uintptr_t visuals::client = (uintptr_t)GetModuleHandle("client.dll");
uintptr_t visuals::engine2 = (uintptr_t)GetModuleHandle("engine2.dll");

void visuals::PlayerESP()
{
	float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + offsets::dwViewMatrix);

	auto localPlayerPawn = *(uintptr_t*)(client + offsets::dwLocalPlayerPawn);

	if (!localPlayerPawn)
		return;

	auto localTeam = *(int*)(localPlayerPawn + offsets::m_iTeamNum);

	auto entityList = *(uintptr_t*)(client + offsets::dwEntityList);

	if (!entityList)
		return;

	for (int i = 1; i < 64; i++) {
		uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
		if (!listEntry)
			continue;

		uintptr_t  playerController = *(uintptr_t*)(listEntry + 112 * (i & 0x1FF));
		if (!playerController)
			continue;

		uint32_t playerPawn = *(uint32_t*)(playerController + offsets::m_hPlayerPawn);
		if (!playerPawn)
			continue;

		uintptr_t listEntry2 = *(uintptr_t*)(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
		if (!listEntry2)
			continue;

		uintptr_t pCSPlayerPawnPointer = *(uintptr_t*)(listEntry2 + 112 * (playerPawn & 0x1FF));
		if (!pCSPlayerPawnPointer)
			continue;

		int health = *(int*)(pCSPlayerPawnPointer + offsets::m_iHealth);
		if (!health || health > 100)
			continue;

		int team = *(int*)(pCSPlayerPawnPointer + offsets::m_iTeamNum);
		if (team == localTeam)	
			continue;

		Vec3 feetPos = *(Vec3*)(pCSPlayerPawnPointer + offsets::m_vOldOrigin);
		Vec3 headPos = { feetPos.x + 0.0f, feetPos.y + 0.0f, feetPos.z + 65.0f };

		Vec2 feet, head;

		if (feetPos.WorlToScreen(feet, ViewMatrix) && headPos.WorlToScreen(head, ViewMatrix)) {
			auto draw = ImGui::GetBackgroundDrawList();

			float height = (feet.y - head.y) * 1.5f;
			float width = height / 1.5f;

			float x = feet.x - width / 2;

			draw->AddRect({ x, head.y }, { x + width, head.y + height }, ImColor(128, 31, 219));

			float healthBarWidth = width * 0.8f;
			float healthBarHeight = 10.0f;
			float healthBarX = x + (width * 0.1f);
			float healthBarY = head.y + height + 0.5f;

			draw->AddRectFilled({ healthBarX, healthBarY }, { healthBarX + healthBarWidth , healthBarY + healthBarHeight }, ImColor(50, 50, 50));

			float heatlhPercentage = std::max(0.0f, std::min(1.0f, (float)health / 100.0f));

			float greenBarWidth = heatlhPercentage * healthBarWidth;

			draw->AddRectFilled({ healthBarX , healthBarY }, { healthBarX + greenBarWidth, healthBarY + healthBarHeight }, ImColor(0, 255, 0));

			
		}
	}
}

void visuals::sniperHelp()
{
	auto draw = ImGui::GetBackgroundDrawList();
	draw->AddRect({ 1920.0f / 2 - 20, 1080.0f / 2 - 20 },  { 1920.0f / 2 + 20, 1080.0f / 2 + 20 }, ImColor(255, 0, 0), 10.0f);
}

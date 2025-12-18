#pragma once

#include <cstddef>

namespace offsets {
	constexpr std::ptrdiff_t dwEntityList = 0x1D13CE8;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1BEEF28;
	constexpr std::ptrdiff_t dwViewMatrix = 0x1E323D0;

	constexpr std::ptrdiff_t dwWindowHeight = 0x8EF844;
	constexpr std::ptrdiff_t dwWindowWidth = 0x8EF840;

	constexpr std::ptrdiff_t m_hPlayerPawn = 0x8FC;
	constexpr std::ptrdiff_t m_iTeamNum = 0x3EB;
	constexpr std::ptrdiff_t m_vOldOrigin = 0x15A0;
	constexpr std::ptrdiff_t m_iHealth = 0x34C;
}

namespace visuals {
	extern uintptr_t client;
	extern uintptr_t engine2;

	void PlayerESP();
	void sniperHelp();
}
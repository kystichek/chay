#include "math.h"
#include <cstdint>
#include "../visuals/visuals.h"

const bool Vec3::WorlToScreen(Vec2& out, float(*ViewMatrix)[4][4]) {
	const float w = (*ViewMatrix)[3][0] * x + (*ViewMatrix)[3][1] * y + (*ViewMatrix)[3][2] * z + (*ViewMatrix)[3][3];

	if (w <= 0.01)
		return false;

	const float invW = 1.0f / w;

	const uint32_t screenWidth = *(uint32_t*)(visuals::engine2 + offsets::dwWindowWidth);
	const uint32_t screenHieght = *(uint32_t*)(visuals::engine2 + offsets::dwWindowHeight);

	out.x = (screenWidth / 2) + (((*ViewMatrix)[0][0] * x + (*ViewMatrix)[0][1] * y + (*ViewMatrix)[0][2] * z + (*ViewMatrix)[0][3]) * invW * (screenWidth / 2));		
	out.y = (screenHieght / 2) - (((*ViewMatrix)[1][0] * x + (*ViewMatrix)[1][1] * y + (*ViewMatrix)[1][2] * z + (*ViewMatrix)[1][3]) * invW * (screenHieght / 2));

	return true;
}
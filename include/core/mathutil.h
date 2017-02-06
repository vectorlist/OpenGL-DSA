#pragma once

#include <algorithm>

namespace math
{
	bool isPowOf2(unsigned int x);
	uint32_t roundUpPow2(uint32_t v);
	int floorInt(float f);
	float gaussian(float x, float w, float falloff = 2.0f);
	int clampInt(int v, int minimum = 0U, int maxium = 1U);
}

inline bool math::isPowOf2(unsigned int x)
{
	return (x & (x - 1)) == 0;
}

inline uint32_t math::roundUpPow2(uint32_t v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

inline int math::floorInt(float f)
{
	return (int)floor(f);
}

//with weight
inline float math::gaussian(float x, float w, float falloff)
{

	return std::max(0.0f,
		expf(-falloff * x * x)
		- expf(-falloff * w * w));
}

inline int math::clampInt(int v, int minimum, int maximum)
{
	return (v > maximum ? maximum : v < minimum ? minimum : v);
}
#pragma once

#include <vector>
#include <rgb.h>
#include <qdebug.h>

struct MapBuffer
{
	MapBuffer(rgba_ptr &src, uint32_t width, uint32_t height)
		: m_pixels(std::move(src)), width(width), height(height) {}

	uint32_t width, height;
	rgba_ptr m_pixels;
	rgba* data() const { return m_pixels.get(); }
};

class Mipmap
{
public:
	Mipmap(uint32_t width, uint32_t height, const uint8_t *pixels, int setlevels = 1);
	~Mipmap();

	uint32_t m_width, m_height;
	std::vector<MapBuffer*> pyramids;

	rgba_ptr resizePixels(const rgba *source,
		int srcWidth, int srcHeight,
		int dstWidth, int dstHeight);

	MapBuffer* mapBuffer(int level);
	uint32_t maxLevels() const { return pyramids.size(); }
};

inline MapBuffer* Mipmap::mapBuffer(int level)
{
	Q_ASSERT(pyramids.size() >= level);
	return pyramids[level];
}
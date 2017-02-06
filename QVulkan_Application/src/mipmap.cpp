#include "Mipmap.h"
#include <vklog.h>
#include <mathutil.h>
#include <color.h>

Mipmap::Mipmap(uint32_t width, uint32_t height, const uint8_t *pixels, int setlevels)
	: m_width(width), m_height(height)
{
	/*BUILD A PRIMARY MIPMAP*/
	rgba_ptr primary = rgba_ptr(new rgba[width * height]);			//we dont need to mutiply 4
	for (uint32_t y = 0; y < height; ++y)
	{
		for (uint32_t x = 0; x < width; ++x)
		{
			uint32_t bitIndex = (y * width + x) * 4;
			uint32_t index = y* width + x;
			rgba p = {
				//vulkan ABGR8888 so we need to inverse that
				pixels[bitIndex + 2],
				pixels[bitIndex + 1],
				pixels[bitIndex + 0],
				pixels[bitIndex + 3]
			};

			primary[index] = p;
		}
	}
	//if not round up ^2 (ex 1129,647....etc)
	if (!math::isPowOf2(width) || !math::isPowOf2(height))
	{
		int pow2Width = math::roundUpPow2(width);
		int pow2Height = math::roundUpPow2(height);

		//set level 0 mipmap by fixed size 
		primary = resizePixels(primary.get(), width, height,
			pow2Width, pow2Height);

		m_width = pow2Width;
		m_height = pow2Height;
	}
	
	//set level 0 mipmap as primary map
	pyramids.push_back(new MapBuffer(primary, m_width, m_height));
	/*LEVELS*/
	int enabledMaxLevels = math::floorInt(std::max(log2((float)width), log2((float)height))) + 1;

	//maxLevels = std::min(maxlevel, enabledMaxLevels);
	int maxlevels = (setlevels > 0) ? std::min(setlevels, enabledMaxLevels) : 0;

	for (int i = 1; i < maxlevels; ++i)
	{
		const MapBuffer* buffer = pyramids[i - 1];
		//reduce size / ^2
		int resizedW = std::max(1U, buffer->width >> 1);
		int resizedH = std::max(1U, buffer->height >> 1);

		rgba_ptr fixedPixels = resizePixels(buffer->data(),
			buffer->width, buffer->height, resizedW, resizedH);
		pyramids.push_back(
			new MapBuffer(fixedPixels, resizedW, resizedH));
	}
}

Mipmap::~Mipmap()
{
	if (!pyramids.size()) return;
	for (auto buffer : pyramids) {
		delete buffer;
	}
}
//Source are rgba pointer and return new unique pointer rgba arrays
//mainly this is normalized color calculate(0.0f- 1.0f)
rgba_ptr Mipmap::resizePixels(const rgba *source,
	int srcWidth, int srcHeight,
	int dstWidth, int dstHeight)
{
	float filterWidth = floor(std::max(2.0f, std::max(
		(float)srcWidth / (float)dstWidth,
		(float)srcHeight / (float)dstHeight)));

	int nSamples = math::floorInt(filterWidth) * 2;

	float* sSampleWeight = new float[dstWidth * nSamples];
	int* sSampleIndex = new int[dstWidth];

	/*ST COORD OF FIXED MAP*/
	/*S == Width */
	for (int s = 0; s < dstWidth; ++s)
	{
		float center = ((float)s + 0.5f) / dstWidth * srcWidth;
		sSampleIndex[s] = math::floorInt(center - filterWidth + 0.5f);

		float weightSum = 0.0f;
		int wOffset = s * nSamples;
		for (int i = 0; i < nSamples; ++i)
		{
			float p = sSampleIndex[s] + 0.5f + i;
			sSampleWeight[wOffset + i] = math::gaussian(p - center, filterWidth);
			weightSum += sSampleWeight[wOffset + i];
		}
		float invW = 1.0f / weightSum;
		for (int i = 0; i < nSamples; ++i)
		{
			sSampleWeight[wOffset + i] *= invW;
		}
	}
	/*T == Height*/
	float* tSampleWeight = new float[dstHeight* nSamples];
	int* tSampleIndex = new int[dstHeight];
	for (int t = 0; t < dstHeight; ++t)
	{
		float center = ((float)t + 0.5f) / dstHeight * srcHeight;
		tSampleIndex[t] = math::floorInt(center - filterWidth + 0.5f);

		float weightSum = 0.0f;
		int wOffset = t * nSamples;
		for (int i = 0; i < nSamples; ++i)
		{
			float p = tSampleIndex[t] + 0.5f + i;
			tSampleWeight[wOffset + i] = math::gaussian(p - center, filterWidth);
			weightSum += tSampleWeight[wOffset + i];
		}
		float invW = 1.0f / weightSum;
		for (int i = 0; i < nSamples; ++i)
		{
			tSampleWeight[wOffset + i] *= invW;
		}
	}

	/*BUILD NEW PIXELS*/
	/*FLOAT ARRAY TO UINT8(UCHAR)*/
	rgba_ptr temp = std::make_unique<rgba[]>(dstWidth * dstHeight);
	float invF = 1.f / 255.f;

	for (int t = 0; t < dstHeight; ++t)
	{
		for (int s = 0; s < dstWidth; ++s)
		{
			int index = t * dstWidth + s;
			Color filterColor = Color(0.f);
			/*SAMPLING*/
			for (int i = 0; i < nSamples; ++i)
			{
				int srcT = math::clampInt(tSampleIndex[t] + i, 0, srcHeight - 1);
				for (int j = 0; j < nSamples; ++j)
				{
					int srcS = math::clampInt(sSampleIndex[s] + j, 0, srcWidth - 1);
					//ok then multiply with filter samples
					float w =
						tSampleWeight[t * nSamples + i] *
						sSampleWeight[s * nSamples + j];

					//expensive but correct
					Color color = Color(
						float(source[srcT * srcWidth + srcS].r) * invF,
						float(source[srcT * srcWidth + srcS].g) * invF,
						float(source[srcT * srcWidth + srcS].b) * invF);

					//TODO : float weight to uint8
					filterColor += w * color;
				}
			}
			rgba pixel = {
				uint8_t(filterColor.r * 255.f),
				uint8_t(filterColor.g * 255.f),
				uint8_t(filterColor.b * 255.f),
				255U
			};
			//LOG << pixel << ENDL;
			temp[index] = pixel;
		}
	}
	delete[] sSampleWeight;
	delete[] sSampleIndex;
	delete[] tSampleWeight;
	delete[] tSampleIndex;
	//pass it to vector directly
	return std::move(temp);
}
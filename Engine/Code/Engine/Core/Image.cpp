#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Thirdparty/stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
#include "Thirdparty/stb/stb_image_write.h"
//////////////////////////////////////////////////////////////////////////
STATIC std::map<std::string, Image*> Image::s_LoadedImages;
////////////////////////////////
Image* Image::AcquireImage(const char* path, int isOpenGlFormat)
{
	auto findedImage = s_LoadedImages.find(std::string(path));
	if (findedImage != s_LoadedImages.end()) {
		return findedImage->second;
	} else {
		Image* createdImage = new Image(path, isOpenGlFormat);
		s_LoadedImages[std::string(path)] = createdImage;
		return createdImage;
	}
}

////////////////////////////////
Image::Image(const char* path, int isOpenGLFormat)
{
	stbi_set_flip_vertically_on_load(isOpenGLFormat);
	int numComponents;
	unsigned char* rawDataWithAlpha = nullptr;
	m_rawData =	stbi_load(path, &m_imageSize.x, &m_imageSize.y, &numComponents, 0);
	m_channels = numComponents;
	GUARANTEE_OR_DIE(m_rawData, Stringf("Image %s can't be loaded", path));
	int numTexels = m_imageSize.x * m_imageSize.y;
	rawDataWithAlpha = new unsigned char[numTexels * 4 * sizeof(unsigned char)];
	m_data.reserve(numTexels);
	for (int texelIndex = 0; texelIndex < numTexels; ++texelIndex) {
		int offset = texelIndex * numComponents;
		int withAlphaOffset = texelIndex * 4;
		unsigned char r = m_rawData[offset];
		unsigned char g = m_rawData[offset + 1];
		unsigned char b = m_rawData[offset + 2];
		unsigned char a = 255u;
		if (numComponents == 4) {
			a = m_rawData[offset + 3];
		}
		rawDataWithAlpha[withAlphaOffset] = r;
		rawDataWithAlpha[withAlphaOffset + 1] = g;
		rawDataWithAlpha[withAlphaOffset + 2] = b;
		rawDataWithAlpha[withAlphaOffset + 3] = a;

		m_data.push_back(Rgba(r, g, b, a));
	}
	stbi_image_free(m_rawData);
	m_rawData = rawDataWithAlpha;
	//stbi_image_free(imageData);
}

////////////////////////////////
Image::Image(int w, int h, const char* name)
	:m_path(name), m_imageSize(w, h)
{
	m_data.resize(w * h);
	m_rawData = new unsigned char[w * h * 4];
}

////////////////////////////////
Image::~Image()
{
	delete[] m_rawData;
}

////////////////////////////////
const Rgba& Image::GetTexel(int texelIndex) const
{
	return m_data[texelIndex];
}

////////////////////////////////
const Rgba& Image::GetTexel(int u, int v) const
{
	return GetTexel(v * m_imageSize.x + u);
}

////////////////////////////////
const IntVec2& Image::GetImageSize() const
{
	return m_imageSize;
}

////////////////////////////////
const std::string& Image::GetPath() const
{
	return m_path;
}

////////////////////////////////
const unsigned char* Image::GetRawImageData() const
{
	return m_rawData;
}

////////////////////////////////
const void Image::WriteFile() const
{
	stbi_write_png(m_path.c_str(), m_imageSize.x, m_imageSize.y, 4, m_rawData, m_imageSize.x * 4);
}

////////////////////////////////
void Image::SetTexelColor(const IntVec2& uv, const Rgba& color)
{
	SetTexelColor(uv.x, uv.y, color);
}

////////////////////////////////
void Image::SetTexelColor(int u, int v, const Rgba& color)
{
	int texelIndex = v * m_imageSize.x + u;
	m_data[texelIndex] = color;
	unsigned char r = (unsigned char)(color.r * 255u);
	unsigned char g = (unsigned char)(color.g * 255u);
	unsigned char b = (unsigned char)(color.b * 255u);
	unsigned char a = (unsigned char)(color.a * 255u);
	int offset = texelIndex * 4;
	m_rawData[offset] = r;
	m_rawData[offset + 1] = g;
	m_rawData[offset + 2] = b;
	m_rawData[offset + 3] = a;
}

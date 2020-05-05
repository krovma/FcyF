#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>
#include <vector>
#include <map>

class Image
{
public:
	static Image* AcquireImage(const char* path, int isOpenGLFormat = 0);
	
	explicit Image(const char* path, int isOpenGLFormat=0);
	explicit Image(int w, int h, const char* name);
	//#Todo explicit Image(const Rgba* data);
	~Image();
	const Rgba& GetTexel(int u, int v) const;
	const Rgba& GetTexel(int texelIndex) const;
	const IntVec2& GetImageSize() const;
	const std::string& GetPath() const;
	const unsigned char* GetRawImageData() const;
	const void WriteFile() const;
	void SetTexelColor(const IntVec2& uv, const Rgba& color);
	void SetTexelColor(int u, int v, const Rgba& color);
	int GetNumChannels() const { return m_channels; };

private:
	std::string m_path;
	IntVec2 m_imageSize;
	std::vector<Rgba> m_data;
	unsigned char* m_rawData;
	int m_channels = 4;

	static std::map<std::string, Image*> s_LoadedImages;
};
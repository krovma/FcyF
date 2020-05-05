#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"

enum ColorSpace
{
	RGBA,
	HSL,
};


class ColorGradient
{
public :
	static ColorGradient WHITE_NOGRADIENT;
	static ColorGradient FADEOUT;
public:
	ColorGradient(ColorSpace space=RGBA) : m_space(space) {}
	ColorGradient(const ColorGradient& copyFrom, ColorSpace space = RGBA) :m_start(copyFrom.m_start), m_end(copyFrom.m_end), m_space(space){}
	ColorGradient(const Rgba& start, const Rgba& end, ColorSpace space = RGBA) : m_start(start), m_end(end), m_space(space) {}
	ColorGradient(const Rgba& noGradient, ColorSpace space = RGBA) : m_start(noGradient), m_end(noGradient), m_space(space) {}
	const Rgba GetColorAt(float position = 0.5f) const;

private:
	Rgba m_start = Rgba::BLACK;
	Rgba m_end = Rgba::WHITE;
	// std::vector<std::pair<float,Rgba>> m_keyPoints;
	ColorSpace m_space=RGBA;
};

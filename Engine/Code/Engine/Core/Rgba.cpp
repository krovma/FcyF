#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <algorithm>
#include <cmath>

//////////////////////////////////////////////////////////////////////////
STATIC const Rgba Rgba::WHITE		(1.0f, 1.0f, 1.0f);
STATIC const Rgba Rgba::BLACK		(0.0f, 0.0f, 0.0f);
STATIC const Rgba Rgba::RED		(1.0f, 0.0f, 0.0f);
STATIC const Rgba Rgba::LIME		(0.0f, 1.0f, 0.0f);
STATIC const Rgba Rgba::BLUE		(0.0f, 0.0f, 1.0f);
STATIC const Rgba Rgba::CYAN		(0.0f, 1.0f, 1.0f);
STATIC const Rgba Rgba::MAGENTA	(1.0f, 0.0f, 1.0f);
STATIC const Rgba Rgba::YELLOW	(1.0f, 1.0f, 0.0f);
STATIC const Rgba Rgba::SILVER	(.75f, .75f, .75f);
STATIC const Rgba Rgba::GRAY		(0.5f, 0.5f, 0.5f);
STATIC const Rgba Rgba::MAROON	(0.5f, 0.0f, 0.0f);
STATIC const Rgba Rgba::OLIVE		(0.5f, 0.5f, 0.0f);
STATIC const Rgba Rgba::GREEN		(0.0f, 0.5f, 0.0f);
STATIC const Rgba Rgba::PURPLE	(0.5f, 0.0f, 0.5f);
STATIC const Rgba Rgba::TEAL		(0.0f, 0.5f, 0.5f);
STATIC const Rgba Rgba::NAVY		(0.0f, 0.0f, 0.5f);

STATIC const Rgba Rgba::TRANSPARENT_BLACK(0.0f, 0.0f, 0.0f, 0.0f);
STATIC const Rgba Rgba::TRANSPARENT_WHITE(1.0f, 1.0f, 1.0f, 0.0f);

STATIC const Rgba Rgba::FLAT	(0.5f, 0.5f, 1.0f, 1.0f);

////////////////////////////////
Rgba Rgba::FromHSL(Vec3 hsl)
{
	Rgba result(0,0,0,1.f);
Fail:
	while (hsl.x >= 360.f) {
		hsl.x -= 360.f;
	}
	int stepBy60 = (int)(hsl.x / 60.f);
	float c = (1.f - fabsf(2.f * hsl.z - 1.f)) * hsl.y;
	float x = c * (1.f - fabsf( fmodf(hsl.x / 60.f, 2.f) - 1.f));
	float m = hsl.z - c * 0.5f;
	switch (stepBy60) {
	case 0:
		result.r = c;
		result.g = x;
		break;
	case 1:
		result.g = c;
		result.r = x;
		break;
	case 2:
		result.g = c;
		result.b = x;
		break;
	case 3:
		result.b = c;
		result.g = x;
		break;
	case 4:
		result.b = c;
		result.r = x;
		break;
	case 5:
		result.r = c;
		result.b = x;
		break;
	default:
		goto Fail;
	}
	result.r += m;
	result.b += m;
	result.g += m;
	return result;
}

Rgba Rgba::FromHSV(Vec3 hsv)
{
	Rgba result(0, 0, 0, 1.f);
Fail:
	while (hsv.x >= 360.f) {
		hsv.x -= 360.f;
	}
	int stepBy60 = (int)(hsv.x / 60.f);
	float c = hsv.x * hsv.z;
	float x = c * (1.f - fabsf(fmodf(hsv.x / 60.f, 2.f) - 1.f));
	float m = hsv.z - c;
	switch (stepBy60) {
	case 0:
		result.r = c;
		result.g = x;
		break;
	case 1:
		result.g = c;
		result.r = x;
		break;
	case 2:
		result.g = c;
		result.b = x;
		break;
	case 3:
		result.b = c;
		result.g = x;
		break;
	case 4:
		result.b = c;
		result.r = x;
		break;
	case 5:
		result.r = c;
		result.b = x;
		break;
	default:
		goto Fail;
	}
	result.r += m;
	result.b += m;
	result.g += m;
	return result;
}

//////////////////////////////////////////////////////////////////////////
Rgba::Rgba()
{
}


//////////////////////////////////////////////////////////////////////////
Rgba::Rgba(float r, float g, float b, float a/*=1.f*/)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{
}

////////////////////////////////
Rgba::Rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a/*=0xFFu*/)
	: r((float)r / 255.f)
	, g((float)g / 255.f)
	, b((float)b / 255.f)
	, a((float)a / 255.f)
{
}

////////////////////////////////
void Rgba::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, ',');
	float tr = 255.f;
	float tg = 255.f;
	float tb = 255.f;
	float ta = 255.f;
	if (splited.size() == 3) {
		tr = (float) std::atof(splited[0].c_str());
		tg = (float) std::atof(splited[1].c_str());
		tb = (float) std::atof(splited[2].c_str());
	} else if (splited.size() == 4) {
		tr = (float) std::atof(splited[0].c_str());
		tg = (float) std::atof(splited[1].c_str());
		tb = (float) std::atof(splited[2].c_str());
		ta = (float) std::atof(splited[3].c_str());
	} else {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to Rgba", FUNCTION, text));
		return;
	}
	r = FloatMap(tr, 0.f, 255.f, 0.f, 1.f);
	g = FloatMap(tg, 0.f, 255.f, 0.f, 1.f);
	b = FloatMap(tb, 0.f, 255.f, 0.f, 1.f);
	a = FloatMap(ta, 0.f, 255.f, 0.f, 1.f);
}

////////////////////////////////
bool Rgba::operator<(const Rgba& rhs) const
{
	return 
		r!=rhs.r ? r < rhs.r
		:(g != rhs.g ? g < rhs.g
		:(b != rhs.b ? b < rhs.b
		:(a != rhs.a ? a < rhs.a
		: false)));
}

////////////////////////////////
const Rgba Rgba::operator+(const Rgba& rhs) const
{
	return Rgba(
		r + rhs.r,
		g + rhs.g,
		b + rhs.b,
		a + rhs.a
	);
}

////////////////////////////////
const Rgba Rgba::operator*(float scale) const
{
	return Rgba(
		scale * r,
		scale * g,
		scale * b,
		scale * a
	);
}

////////////////////////////////
Vec3 Rgba::ToHSL() const
{
	float cmax = std::max(r, std::max(g, b));
	float cmin = std::min(r, std::min(g, b));
	float delta = cmax - cmin;

	float hue;
	float saturation;
	float lightness;

	lightness = (cmax + cmin) * 0.5f;

	if (delta == 0.f) {
		hue = 0.f;
		saturation = 0.f;
	} else {
		saturation = delta / (1.f - fabsf(2.f * lightness - 1.f));
		float cof;
		if (cmax == r) {
			cof = (g - b) / delta + 6.f;
		} else if (cmax == g) {
			cof = (b - r) / delta + 2.f;
		} else {
			cof = (r - g) / delta + 4.f;
		}
		if (cof > 6.f)
			cof -= 6.f;
		hue = cof * 60.f;
	}
	return Vec3(hue, saturation, lightness);
}

////////////////////////////////
Vec3 Rgba::ToHSV() const
{
	float cmax = std::max(r, std::max(g, b));
	float cmin = std::min(r, std::min(g, b));
	float delta = cmax - cmin;

	float hue;
	float saturation;
	float value;

	value = cmax;

	if (delta == 0.f) {
		hue = 0.f;
		saturation = 0.f;
	} else {
		saturation = delta / value;
		float cof;
		if (cmax == r) {
			cof = (g - b) / delta + 6.f;
		} else if (cmax == g) {
			cof = (b - r) / delta + 2.f;
		} else {
			cof = (r - g) / delta + 4.f;
		}
		if (cof > 6.f)
			cof -= 6.f;
		hue = cof * 60.f;
	}
	return Vec3(hue, saturation, value);
}

////////////////////////////////
const bool Rgba::operator==(const Rgba& rhs) const
{
	return (
		r == rhs.r
	&&	g == rhs.g
	&&	b == rhs.b
	&&	a == rhs.a
	);
}

const Rgba operator*(float scale, const Rgba & color)
{
	return color * scale;
}

#pragma once
#include "Engine/Math/Vec3.hpp"

struct Rgba
{
public:
	// statics
	
	// Basic RGB color from https://www.rapidtables.com/web/color/RGB_Color.html
	static const Rgba WHITE;		//0xFFFFFF FF(by Default)
	static const Rgba BLACK;		//0x000000
	static const Rgba RED;			//0xFF0000
	static const Rgba LIME;			//0x00FF00
	static const Rgba BLUE;			//0x0000FF
	static const Rgba CYAN;			//0x00FFFF
	static const Rgba MAGENTA;		//0xFF00FF
	static const Rgba YELLOW;		//0xFFFF00
	static const Rgba SILVER;		//0xC0C0C0
	static const Rgba GRAY;			//0x808080
	static const Rgba MAROON;		//0x800000
	static const Rgba OLIVE;		//0x808000
	static const Rgba GREEN;		//0x008000
	static const Rgba PURPLE;		//0x800080
	static const Rgba TEAL;			//0x008080
	static const Rgba NAVY;			//0x000080

	static const Rgba TRANSPARENT_BLACK;	//0x00000000
	static const Rgba TRANSPARENT_WHITE;	//0xFFFFFF00

	static const Rgba FLAT;

	static Rgba FromHSL(Vec3 hsl);
	static Rgba FromHSV(Vec3 hsv);
	Rgba();
	explicit Rgba(float r, float g, float b, float a=1.f);
	explicit Rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a=0xFFu);


	/// Format:
	/// byte, byte, byte[, byte](support[0, 255])
	/// #Todo: #ff0000[ff]
	/// #Todo: Named Color
	void SetFromText(const char* text);

	/// For std::map, Remove it when migrate to hashmap
	bool operator< (const Rgba& rhs) const;

	/// For lerping colors
	const Rgba operator+ (const Rgba& rhs) const;
	const Rgba operator* (float scale) const;
	const bool operator== (const Rgba& rhs) const;
	friend const Rgba operator* (float scale, const Rgba& color);

	Vec3 ToHSL() const;
	Vec3 ToHSV() const;

public:
	float r = 1.f;
	float g = 1.f;
	float b = 1.f;
	float a = 1.f;
};
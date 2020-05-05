#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>
#include <string>

class TextureView2D;

enum TextDrawMode
{
	TEXT_DRAW_OVERFLOW,
	TEXT_DRAW_SHRINK_TO_FIT,

	NUM_TEXT_DRAW_MODES
};

class BitmapFont
{
	friend class RenderContext;
private:
	explicit BitmapFont(const char* fontName, const Texture2D* fontTexture);
public:
	static const Vec2 ALIGNMENT_TOPLEFT;
	static const Vec2 ALIGNMENT_TOPCENTER;
	static const Vec2 ALIGNMENT_TOPRIGHT;
	static const Vec2 ALIGHMENT_LEFT;
	static const Vec2 ALIGNMENT_CENTER;
	static const Vec2 ALIGNMENT_RIGHT;
	static const Vec2 ALIGNMENT_BOTTOMLEFT;
	static const Vec2 ALIGNMENT_BOTTOMCENTER;
	static const Vec2 ALIGNMENT_BOTTOMRIGHT;

public:
	const TextureView2D* GetFontTexture() const { return m_fontSpriteSheet.GetTextureView(); }

	/// Does not check \r, \n or \t
	float GetSingleLineTextWidth(const std::string& text, float cellHeight, float aspect = 1.f) const;

	void AddTextInBox(std::vector<Vertex_PCU>& vertices
						, const std::string& text
						, const AABB2& box
						, const Vec2& alienment
						, float cellHeight
						, const Rgba& tint=Rgba::WHITE
						, TextDrawMode drawMode=TEXT_DRAW_OVERFLOW
						, int maxGlyphs = 99999999
						, float cellAspect = 1.f) const;


	void AddVertsForText2D(std::vector<Vertex_PCU>& vertices
							, const Vec2& textMin
							, float cellHeight
							, const std::string& text
							, const Rgba& tint=Rgba::WHITE
							, int maxGlyphs = 9999999
							, float cellAspect = 1.f) const;
	float GetGlyphAspect(unsigned int unicode) const;

private:
	std::string m_fontName;
	SpriteSheet m_fontSpriteSheet;
};
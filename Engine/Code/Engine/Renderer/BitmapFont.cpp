#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/VertexUtils.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC const Vec2 BitmapFont::ALIGNMENT_TOPLEFT = Vec2(0.f, 0.f);
STATIC const Vec2 BitmapFont::ALIGNMENT_TOPCENTER = Vec2(0.5f, 0.f);
STATIC const Vec2 BitmapFont::ALIGNMENT_TOPRIGHT = Vec2(1.f, 0.f);
STATIC const Vec2 BitmapFont::ALIGHMENT_LEFT = Vec2(0.f, 0.5f);
STATIC const Vec2 BitmapFont::ALIGNMENT_CENTER = Vec2(0.5f, 0.5f);
STATIC const Vec2 BitmapFont::ALIGNMENT_RIGHT = Vec2(1.f, 0.5f);
STATIC const Vec2 BitmapFont::ALIGNMENT_BOTTOMLEFT = Vec2(0.f, 1.f);
STATIC const Vec2 BitmapFont::ALIGNMENT_BOTTOMCENTER = Vec2(0.5f, 1.f);
STATIC const Vec2 BitmapFont::ALIGNMENT_BOTTOMRIGHT = Vec2(1.f, 1.f);
////////////////////////////////
BitmapFont::BitmapFont(const char* fontName, const Texture2D* fontTexture)
	: m_fontName(fontName)
	, m_fontSpriteSheet(fontTexture, IntVec2(16, 16))
{
}


////////////////////////////////
float BitmapFont::GetSingleLineTextWidth(const std::string& text, float cellHeight,float aspect/*1.f*/) const
{
	float cellWidth = cellHeight * aspect * GetGlyphAspect(text[0]);
	return cellWidth * text.length();
}

////////////////////////////////
void BitmapFont::AddTextInBox(std::vector<Vertex_PCU>& vertices
								, const std::string& text
								, const AABB2& box
								, const Vec2& alienment
								, float cellHeight
								, const Rgba& tint/*=Rgba::WHITE */
								, TextDrawMode drawMode/*=TEXT_DRAW_OVERFLOW*/
								, int maxGlyphs/*=99999999*/
								, float cellAspect /*= 1.f*/) const
{
	std::vector<std::string> splited = Split(text.c_str(), '\n');
	Vec2 textAreaSize(-1, -1);
	for (const std::string& each : splited) {
		float width = GetSingleLineTextWidth(each, cellHeight, cellAspect);
		if (width > textAreaSize.x)
			textAreaSize.x = width;
	}
	textAreaSize.y = (float)splited.size() * cellHeight;

	if (drawMode == TEXT_DRAW_SHRINK_TO_FIT) {
		if (textAreaSize.y > box.GetHeight()) {
			float scale = box.GetHeight() / textAreaSize.y;
			textAreaSize *= scale;
			cellHeight *= scale;
		}
		if (textAreaSize.x > box.GetWidth()) {
			float scale = box.GetWidth() / textAreaSize.x;
			textAreaSize *= scale;
			cellHeight *= scale;
		}
	}

	Vec2 textAreaMin;
	textAreaMin.x = box.Min.x + (box.GetWidth() - textAreaSize.x) * alienment.x;
	textAreaMin.y = box.Min.y + (box.GetHeight() - textAreaSize.y) * alienment.y;
	
	int glyphsDrawn = 0;
	for (int i = 1; i <= splited.size(); ++i) {
		int textIndex = i - 1;
		Vec2 textMin;
		textMin.x = (textAreaSize.x - GetSingleLineTextWidth(splited[textIndex], cellHeight, cellAspect)) * alienment.x;
		textMin.y = (splited.size() - i) * cellHeight;
		textMin += textAreaMin;
		
		int glyphsToDraw = (int)splited[textIndex].length();
		if (glyphsToDraw > maxGlyphs - glyphsDrawn) {
			glyphsToDraw = maxGlyphs - glyphsDrawn;
		}
		AddVertsForText2D(vertices, textMin, cellHeight,
			splited[textIndex], tint, glyphsToDraw,cellAspect);
		glyphsDrawn += glyphsToDraw;
		if (glyphsDrawn >= maxGlyphs)
			break;
	}
}

////////////////////////////////
void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertices
									, const Vec2& textMin
									, float cellHeight
									, const std::string& text
									, const Rgba& tint/*=Rgba::WHITE */
									, int maxGlyphs /*= 99999999*/
									, float cellAspect /*= 1.f*/) const
{
	if (maxGlyphs < 1) {
		return;
	}
	float cellWidth = cellHeight * cellAspect/* * GetGlyphAspect(text[0])*/;
	// 256 = 16 * 16
	Vec2 max = textMin + Vec2(cellWidth, cellHeight);
	AABB2 carriage(textMin, max);
	Vec2 txUVmin;
	Vec2 txUVmax;
	m_fontSpriteSheet.GetSpriteDef(text[0]).GetUVs(txUVmin, txUVmax);
	AddVerticesOfAABB2D(vertices, carriage, tint, txUVmin, txUVmax);

	for (int i = 1; i < (int)text.size() && i < maxGlyphs; ++i) {
		carriage.Min = Vec2(carriage.Max.x, carriage.Min.y);
		cellWidth = cellHeight * cellAspect * GetGlyphAspect(text[i]);
		carriage.Max = carriage.Min + Vec2(cellWidth, cellHeight);
		m_fontSpriteSheet.GetSpriteDef(text[i]).GetUVs(txUVmin, txUVmax);
		AddVerticesOfAABB2D(vertices, carriage, tint, txUVmin, txUVmax);
	}
}

////////////////////////////////
float BitmapFont::GetGlyphAspect(unsigned int unicode) const
{
	UNUSED(unicode);
	return 1.0f;
}

#endif
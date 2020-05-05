#pragma once
#include "Engine/Math/Vec2.hpp"

//////////////////////////////////////////////////////////////////////////
class SpriteDef
{
public:
	SpriteDef() = default;
	SpriteDef& operator=(const SpriteDef& copyfrom);
	explicit SpriteDef(int spriteIndex, const Vec2& uvBottomLeft = Vec2::ZERO, const Vec2& uvTopRight = Vec2::ONE);
	void GetUVs(Vec2& out_uvBottomLeft, Vec2& out_uvTopRight) const;
private:
	int m_spriteIndex;
	Vec2 m_uvBottomLeft = Vec2::ZERO;
	Vec2 m_uvTopRight = Vec2::ONE;
};
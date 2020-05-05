#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Renderer/SpriteDef.hpp"

////////////////////////////////
SpriteDef::SpriteDef(int spriteIndex, const Vec2& uvBottomLeft /*= Vec2::ZERO*/, const Vec2& uvTopRight /*= Vec2::ONE*/)
	: m_spriteIndex(spriteIndex)
	, m_uvBottomLeft(uvBottomLeft)
	, m_uvTopRight(uvTopRight)
{
}

////////////////////////////////
SpriteDef& SpriteDef::operator=(const SpriteDef& copyfrom)
{
	m_spriteIndex = copyfrom.m_spriteIndex;
	m_uvBottomLeft = copyfrom.m_uvBottomLeft;
	m_uvTopRight = copyfrom.m_uvTopRight;
	return *this;
}

////////////////////////////////
void SpriteDef::GetUVs(Vec2& out_uvBottomLeft, Vec2& out_uvTopRight) const
{
	out_uvBottomLeft = m_uvBottomLeft;
	out_uvTopRight = m_uvTopRight;
}

#endif
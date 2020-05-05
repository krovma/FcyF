#include "Engine/Renderer/SpriteAnimationDef.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Core/XmlUtils.hpp"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
///// Legacy Code
// 
// 
// ////////////////////////////////
// SpriteAnimationDef::SpriteAnimationDef(const SpriteSheet& spriteSheet
// 										, int startFrameIndex
// 										, int endFrameIndex
// 										, float durationSeconds
// 										, SpriteAnimationMode animationMode /*= SpriteAnimationMode::SPRITE_ANIMATION_LOOP*/)
// 	:m_spriteSheet(spriteSheet)
// 	,m_startIndex(startFrameIndex)
// 	,m_endIndex(endFrameIndex)
// 	,m_duration(durationSeconds)
// 	,m_animationMode(animationMode)
// {
// 	if (m_animationMode != SPRITE_ANIMATION_PINGPONG) {
// 		m_frameTotal = m_endIndex - m_startIndex + 1;
// 	} else {
// 		m_frameTotal = (m_endIndex - m_startIndex + 1 - 1) * 2;
// 	}
// }
// 
// ////////////////////////////////
// const SpriteDef& SpriteAnimationDef::GetFrameAtTime(float seconds) const
// {
// 	int cycleDone = (int)(seconds / m_duration);
// 	int spriteIndex = -1;
// 
// 	if (m_animationMode == SPRITE_ANIMATION_ONCE && cycleDone > 0) {
// 		spriteIndex = m_endIndex;
// 	} else {
// 		seconds -= m_duration * (float)cycleDone;
// 		int frame = (int)(seconds / GetDurationOfFrame(0));
// 
// 		if (m_animationMode == SPRITE_ANIMATION_PINGPONG) {
// 			int baseFrames = m_endIndex - m_startIndex + 1;
// 			if (frame < baseFrames) {
// 				spriteIndex = frame + m_startIndex;
// 			} else {
// 				int backFromTheEnd = frame - baseFrames + 1;
// 				spriteIndex = m_endIndex - backFromTheEnd;
// 			}
// 		} else {
// 			spriteIndex = frame + m_startIndex;
// 		}
// 	}
// 	return m_spriteSheet.GetSpriteDef(spriteIndex);
// }
// 
// ////////////////////////////////
// float SpriteAnimationDef::GetDurationOfFrame(int frameNumber) const
// {
// 	UNUSED(frameNumber);
// 	return m_duration / (float)m_frameTotal;
// }

STATIC std::map<std::string, SpriteAnimationDef*> SpriteAnimationDef::s_loadedAnimations;

////////////////////////////////
SpriteAnimation* SpriteAnimationDef::GetAnimaion(const std::string& id)
{
	auto found = m_animations.find(id);
	if (found == m_animations.end()) {
		ERROR_AND_DIE("No animation in Sprite AnimationDef");
	}
	return m_animations[id];
}

////////////////////////////////
SpriteAnimationDef::~SpriteAnimationDef()
{
	for (auto each : m_animations)
	{
		delete each.second;
	}
}
////////////////////////////////
SpriteAnimationDef* SpriteAnimationDef::AcquireAnimationDefFromFile(const std::string& path)
{
	auto found = s_loadedAnimations.find(path);
	if (found != s_loadedAnimations.end()) {
		return found->second;
	}
	XmlElement* xml;
	ParseXmlFromFile(xml, path.c_str());
	SpriteAnimationDef* created = LoadAnimationFromXml(*xml);
	s_loadedAnimations[path] = created;
	return created;
}


////////////////////////////////
SpriteAnimationDef* SpriteAnimationDef::LoadAnimationFromXml(const XmlElement& xml)
{
	const std::string animSetId = ParseXmlAttr(xml, "id", "default");
	const std::string texturePath = ParseXmlAttr(xml, "texture", "Data/Images/Test_SpriteSheet4x4.png");
	const IntVec2 size = ParseXmlAttr(xml, "size", IntVec2(32, 32));
	const int ppu = ParseXmlAttr(xml, "ppu", 64);
	const Vec2 pivot = ParseXmlAttr(xml, "pivot", Vec2(0.5f, 0.5f));

	auto newDef = new SpriteAnimationDef();
	const Texture2D* texture = g_theRenderer->AcquireTextureFromFile(texturePath.c_str(), 0);
	const IntVec2& textureSize = texture->GetTextureSize();
	const IntVec2 layout(textureSize.x / size.x, textureSize.y / size.y);
	newDef->m_spriteSheet = new SpriteSheet(texture, layout);
	newDef->m_frameSize = size;
	newDef->m_pivot = pivot;
	newDef->m_ppu = ppu;
	newDef->m_frameAABB = AABB2(0, 0, (float)size.x / (float)ppu, (float)size.y / (float)ppu);
	
	auto pAnim = xml.FirstChildElement("anim");
	while (pAnim != nullptr) {
		const std::string animID = ParseXmlAttr(*pAnim, "id", animSetId + "unknown");
		if (newDef->m_animations.find(animID) != newDef->m_animations.end()) {
			ERROR_RECOVERABLE("Exist AnimID!");
			continue;
		}
		const IntVec2 cell = ParseXmlAttr(*pAnim, "cell", IntVec2::ZERO);
		const int frameCount = ParseXmlAttr(*pAnim, "frameCount", 1);
		const float frameTime = ParseXmlAttr(*pAnim, "frameTime", 0.05f);
		const bool once = ParseXmlAttr(*pAnim, "once", false);
		SpriteAnimation* newAnimation = new SpriteAnimation();
		newAnimation->m_def = newDef;
		newAnimation->m_startCell = newDef->m_spriteSheet->GetIndexFromCellCoord(cell);
		newAnimation->m_frameCount = frameCount;
		newAnimation->m_frameTime = frameTime;
		newAnimation->m_animationMode = (once ? SPRITE_ANIMATION_ONCE : SPRITE_ANIMATION_LOOP);


		auto pEvent = pAnim->FirstChildElement("event");
		while (pEvent != nullptr) {
			SpriteAnimation::_FrameEvent ev;
			ev.m_frame = ParseXmlAttr(*pEvent, "frame", 0);
			ev.m_action = ParseXmlAttr(*pEvent, "action", "");
			ev.m_bindToEntity = ParseXmlAttr(*pEvent, "bindToEntity", false);
			newAnimation->m_events.push_back(ev);
			pEvent = pEvent->NextSiblingElement("event");
		}

		newDef->m_animations[animID] = newAnimation;
		

		pAnim = pAnim->NextSiblingElement("anim");
	}
	return newDef;
}

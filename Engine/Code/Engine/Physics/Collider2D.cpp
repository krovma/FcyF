#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Event/EventSystem.hpp"
////////////////////////////////
Collider2D::Collider2D(Collider2DType colliderType, Rigidbody2D* rigidbody)
	:m_type(colliderType), m_rigidbody(rigidbody)
{
}

////////////////////////////////
void Collider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	UNUSED(renderColor);
	UNUSED(renderer);
	// Do nothing
}

////////////////////////////////
Collision2D Collider2D::GetCollisionWith(const Collider2D* other) const
{
	Collision2D collision;
	GetCollision(collision, this, other);
	return collision;
}

void Collider2D::AddInside(Collider2D* c)
{
	for(auto each : m_insideList) {
		if (each == c) {
			return;
		}
	}
	m_insideList.push_back(c);
	if (!onEnterEvent.empty()) {
		NamedStrings p;
		p.Set("collider", Stringf("%I64d", c));
		p.Set("into", Stringf("%I64d", this));
		g_Event->Trigger(onEnterEvent, p);
	}
}

void Collider2D::RemoveInside(Collider2D* c)
{
	for (auto it = m_insideList.begin(); it != m_insideList.end(); ++it) {
		if (*it == c) {
			if (!onLeaveEvent.empty()) {
				NamedStrings p;
				p.Set("collider", Stringf("%I64d", c));
				p.Set("from", Stringf("%I64d", this));
				g_Event->Trigger(onLeaveEvent, p);
			}
			m_insideList.erase(it);
			return;
		}
	}
}

void Collider2D::CleanUpInside()
{
	for (auto it = m_insideList.begin(); it != m_insideList.end(); ) {
		if ((*it)->m_destroied) {
			if (!(*it)->onLeaveEvent.empty()) {
				NamedStrings p;
				p.Set("collider", Stringf("%I64d", (*it)));
				p.Set("from", Stringf("%I64d", this));
				g_Event->Trigger((*it)->onLeaveEvent, p);
				it = m_insideList.erase(it);
				continue;
			}
		}
		++it;
	}
}

void Collider2D::MarkDestroy()
{
	m_destroied = true;

	if (m_isTrigger) {
		for (auto it = m_insideList.begin(); it != m_insideList.end(); ++it) {
				if (!(*it)->onLeaveEvent.empty()) {
					NamedStrings p;
					p.Set("collider", Stringf("%I64d", (*it)));
					p.Set("from", Stringf("%I64d", this));
					g_Event->Trigger((*it)->onLeaveEvent, p);
				}
		}
		m_insideList.clear();
	}
}

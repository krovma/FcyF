#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <algorithm>

Clock* g_theMasterClock = new Clock(nullptr);

Clock::Clock()
	: m_parent(g_theMasterClock)
{
	g_theMasterClock->AddChild(this);
}

Clock::Clock(Clock* parent)
	:m_parent(parent)
{
	if (parent != nullptr) {
		parent->AddChild(this);
	}
}

Clock::~Clock()
{
	if (m_parent == nullptr)
		m_parent = g_theMasterClock;
	for (auto each : m_children) {
		each->SetParent(m_parent);
	}
}

void Clock::SetParent(Clock* parent)
{
	if (m_parent != nullptr) {
		m_parent->RemoveChild(this);
	}
	m_parent = parent;
	parent->AddChild(this);
}

void Clock::AddChild(Clock* clock)
{
	for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (*iter == clock) {
			return;
		}
	}
	m_children.push_back(clock);
}
void Clock::RemoveChild(Clock* clock)
{
	for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if (*iter == clock) {
			m_children.erase(iter);
			return;
		}
	}

}

void Clock::Step(double deltaTime)
{
	if (m_pause) {
		m_frameTime = 0.f;
	} else {
		deltaTime *= m_scale;
		deltaTime = Clamp(deltaTime, 0.0, m_frameLimit);
		m_frameTime = (float)deltaTime;
	}
	for (auto each : m_children) {
		each->Step(m_frameTime);
	}
}

float Clock::GetScale() const
{
	return m_scale;
}

void Clock::SetScale(const float scale)
{
	m_scale = scale;
}

float Clock::GetFrameLimit() const
{
	return (float)m_frameLimit;
}

void Clock::SetFrameLimit(const float frameLimit)
{
	m_frameLimit = frameLimit;
}

void Clock::SetFPS(float fps)
{
	m_frameLimit = 1.0 / (double)fps;
}

void Clock::Pause()
{
	m_pause = true;
}

void Clock::Unpause()
{
	m_pause = false;
}

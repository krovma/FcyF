#pragma once
#include <vector>

class Clock
{
public:
	Clock();
	Clock(Clock* parent);
	~Clock();

	void SetParent(Clock* parent);
	void AddChild(Clock* clock);
	void RemoveChild(Clock* clock);
	void Step(double deltaTime);


private:
	std::vector<Clock*> m_children;
	Clock* m_parent;
	
	float m_scale = 1.0;
public:
	float GetScale() const;
	void SetScale(const float scale);
private:
	float m_frameTime = 0.f;
	double m_frameLimit = 1.0 / 30.0;
	bool m_pause = false;

public:
	float GetFrameLimit() const;
	void SetFrameLimit(const float frameLimit);
	float GetFrameTime() const { return m_frameTime; }
	void SetFPS(float fps);
	void Pause();
	void Unpause();
};

extern Clock* g_theMasterClock;
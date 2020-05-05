#pragma once
#include "Engine/Core/EngineCommon.hpp"

#define UI_USING_IMGUI

class UISystem_Imgui;
class UISystem_Canvas;

#if defined(UI_USING_IMGUI)
using UISystem = UISystem_Imgui;
#elif defined(UI_USING_CANVAS)
using UISystem = UISystem_Canvas;
#endif

class RenderContext;
class WindowContext;

class UISystem_Imgui
{
public:
	void Startup(WindowContext* window, RenderContext* renderer);
	void Shutdown();
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();
private:
	WindowContext* m_window = nullptr;
	RenderContext* m_renderer = nullptr;
};

extern UISystem* g_theUI;
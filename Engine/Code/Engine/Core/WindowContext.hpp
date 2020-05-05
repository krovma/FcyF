#pragma once

#include "Engine/Math/IntVec2.hpp"
#include <string>
//////////////////////////////////////////////////////////////////////////
enum MouseInputMode
{
	MOUSE_INPUT_ABSOLUTE,
	MOUSE_INPUT_RELATIVE
};

using WindowsCallback = bool(void*, unsigned int message, unsigned int wparam, unsigned int lparam);

class WindowContext
{
public:
	WindowContext();
	~WindowContext();
	WindowsCallback* GameWindowProc;
	void* m_hWnd = nullptr;

	bool IsOpen() const { return m_hWnd != nullptr; }
	void Create(const std::string& title, float clientAspect, float maxClientFractionOfDesktop, WindowsCallback* gameProc);
	void Close();

	void BeginFrame();
	void EndFrame();
	IntVec2 GetClientCenter() const;
	
	void SetMouseInputMode(MouseInputMode mode);

	IntVec2 GetClientMousePosition();
	void SetClientMousePosition(const IntVec2& mousePosition);
	void SetClientMousePosition(int x, int y);

	IntVec2 GetClientMouseRelativeMovement();
	IntVec2 GetClientResolution() const;

	bool IsMouseLocked() const { return m_mouseLockCount > 0; }
	void LockMouse();
	void UnlockMouse();
	void ShowMouse();
	void HideMouse();
	
	bool IsFocused() const;


private:
	int m_mouseLockCount = 0;
	IntVec2 m_windowResolution;
	IntVec2 m_lastMousePosition;
	IntVec2 m_currentMousePosition;
	MouseInputMode m_mouseInputMode = MOUSE_INPUT_ABSOLUTE;
};
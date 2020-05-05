#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/WindowContext.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
static constexpr const TCHAR* __WINDOW_CONTEXT_CLASS_NAME = TEXT("WindowContext");
static void __LockMouseToWindow(HWND hWnd) 
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	POINT clientOrigin{ 0, 0 };
	ClientToScreen(hWnd, &clientOrigin);
	clientRect.left += clientOrigin.x;
	clientRect.right += clientOrigin.x;
	clientRect.top += clientOrigin.y;
	clientRect.bottom += clientOrigin.y;
	ClipCursor(&clientRect);
}
static LRESULT CALLBACK __EngineWindowProc(HWND hWnd, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	WindowContext* windowContext = (WindowContext*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (wmMessageCode) {
	case WM_ACTIVATE: {
		WORD activeWord = LOWORD(wParam);
		bool isActive = (activeWord == WA_ACTIVE);
		if (isActive && windowContext->IsMouseLocked()) {
			__LockMouseToWindow(hWnd);
		}
		break;
	}
	default: {
	}
	}

	bool consumed = false;
	if (windowContext != nullptr) {
		consumed = windowContext->GameWindowProc(hWnd, wmMessageCode, (unsigned int)wParam, (unsigned int)lParam);
	}

	if (!consumed) {
		return DefWindowProc(hWnd, wmMessageCode, wParam, lParam);
	} else {
		return 0;
	}
}
static int __registerCount = 0;
static void __RegisterGameWindowClass()
{
	WNDCLASSEX windowClassDesc;
	memset(&windowClassDesc, 0, sizeof(windowClassDesc));
	windowClassDesc.cbSize = sizeof(windowClassDesc);
	windowClassDesc.style = CS_OWNDC;
	windowClassDesc.lpfnWndProc = (WNDPROC)(__EngineWindowProc);
	windowClassDesc.hInstance = GetModuleHandle(nullptr);
	windowClassDesc.hIcon = nullptr;
	windowClassDesc.hCursor = nullptr;
	windowClassDesc.lpszClassName = __WINDOW_CONTEXT_CLASS_NAME;
	RegisterClassEx(&windowClassDesc);
}

////////////////////////////////
WindowContext::WindowContext()
{
	if (__registerCount <= 0) {
		__RegisterGameWindowClass();
	}
	++__registerCount;
}

////////////////////////////////
WindowContext::~WindowContext()
{
	Close();
	--__registerCount;
	if (__registerCount <= 0) {
		UnregisterClass(__WINDOW_CONTEXT_CLASS_NAME, GetModuleHandle(nullptr));
	}
}

////////////////////////////////
void WindowContext::Create(const std::string& title, float clientAspect, float maxClientFractionOfDesktop, WindowsCallback* gameProc)
{
	GUARANTEE_OR_DIE(!IsOpen(), "Window is already opened");
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if (clientAspect > desktopAspect) {
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	} else {
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}
	m_windowResolution = IntVec2((int)clientWidth, (int)clientHeight);
	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, title.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		__WINDOW_CONTEXT_CLASS_NAME,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		::GetModuleHandle(nullptr),
		nullptr);

	ASSERT_OR_DIE(hwnd != nullptr, "Failed to create window");

	m_hWnd = (void*)hwnd;
	GameWindowProc = gameProc;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	HCURSOR cursor = LoadCursor(nullptr, IDC_ARROW);
	SetCursor(cursor);

	// Start the game with 0 mouse movement; 
	m_lastMousePosition = GetClientMousePosition();
	m_currentMousePosition = GetClientMousePosition();
}

////////////////////////////////
void WindowContext::Close()
{
	if (!IsOpen()) {
		return;
	}
	CloseWindow((HWND)m_hWnd);
	m_hWnd = nullptr;
}

////////////////////////////////
void WindowContext::BeginFrame()
{
	MSG queuedMessage;
	while (true) {
		const BOOL haveMessage = PeekMessage(&queuedMessage, nullptr, 0, 0, PM_REMOVE);
		if (!haveMessage) {
			break;
		}
		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
	m_lastMousePosition = m_currentMousePosition;
	m_currentMousePosition = GetClientMousePosition();

	if (m_mouseInputMode == MOUSE_INPUT_RELATIVE && IsFocused()) {
		IntVec2 center = GetClientCenter();
		SetClientMousePosition(center);
		m_lastMousePosition = GetClientMousePosition();
	}
}

////////////////////////////////
void WindowContext::EndFrame()
{

}

////////////////////////////////
IntVec2 WindowContext::GetClientCenter() const
{
	RECT clientRect;
	GetClientRect((HWND)m_hWnd, &clientRect);

	IntVec2 center;
	center.x = (clientRect.left + clientRect.right) / 2;
	center.y = (clientRect.top + clientRect.bottom) / 2;

	return center;
}

////////////////////////////////
void WindowContext::SetMouseInputMode(MouseInputMode mode)
{
	m_mouseInputMode = mode;
	if (mode == MOUSE_INPUT_RELATIVE) {
		IntVec2 center = GetClientCenter();
		m_currentMousePosition = center;
		m_lastMousePosition = center;
		SetClientMousePosition(center);
	}
}

////////////////////////////////
IntVec2 WindowContext::GetClientMousePosition()
{
	POINT desktopPosition;
	if (!GetCursorPos(&desktopPosition)) {
		desktopPosition.x = 0;
		desktopPosition.y = 0;
	}
	ScreenToClient((HWND)m_hWnd, &desktopPosition);
	//RECT...
	return IntVec2(desktopPosition.x, desktopPosition.y);
}

////////////////////////////////
void WindowContext::SetClientMousePosition(const IntVec2& mousePosition)
{
	SetClientMousePosition(mousePosition.x, mousePosition.y);
}

////////////////////////////////
void WindowContext::SetClientMousePosition(int x, int y)
{
	POINT screen{ x, y };
	ClientToScreen((HWND)m_hWnd, &screen);
	SetCursorPos(screen.x, screen.y);
}

////////////////////////////////
IntVec2 WindowContext::GetClientMouseRelativeMovement()
{
	return m_currentMousePosition - m_lastMousePosition;
}

////////////////////////////////
IntVec2 WindowContext::GetClientResolution() const
{
	return m_windowResolution;
}

////////////////////////////////
void WindowContext::LockMouse()
{
	++m_mouseLockCount;
	if (m_mouseLockCount > 0) {
		__LockMouseToWindow((HWND)m_hWnd);
	}
	m_lastMousePosition = GetClientMousePosition();
	m_currentMousePosition = GetClientMousePosition();
}

////////////////////////////////
void WindowContext::UnlockMouse()
{
	--m_mouseLockCount;
	if (m_mouseLockCount <= 0) {
		m_mouseLockCount = 0;
		ClipCursor(nullptr);
	}
}

////////////////////////////////
void WindowContext::ShowMouse()
{
	ShowCursor(TRUE);
}

////////////////////////////////
void WindowContext::HideMouse()
{
	ShowCursor(FALSE);
}

////////////////////////////////
bool WindowContext::IsFocused() const
{
	return GetActiveWindow() == (HWND)m_hWnd;
}

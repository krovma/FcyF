#pragma once
#include "XboxController.hpp"
//////////////////////////////////////////////////////////////////////////
constexpr int MAX_XBOX_CONTROLLERS = 4;
/////////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern const unsigned char KEY_ESC;
extern const unsigned char KEY_SPACE;
extern const unsigned char KEY_UPARROW;
extern const unsigned char KEY_LEFTARROW;
extern const unsigned char KEY_DOWNARROW;
extern const unsigned char KEY_RIGHTARROW;
extern const unsigned char KEY_F1;
extern const unsigned char KEY_F2;
extern const unsigned char KEY_F3;
extern const unsigned char KEY_F4;
extern const unsigned char KEY_F6;
extern const unsigned char KEY_F8;
extern const unsigned char KEY_A;
extern const unsigned char KEY_D;
extern const unsigned char KEY_E;
extern const unsigned char KEY_N;
extern const unsigned char KEY_M;
extern const unsigned char KEY_R;
extern const unsigned char KEY_S;
extern const unsigned char KEY_W;
extern const unsigned char KEY_PERIOD;
extern const unsigned char KEY_COMMA;
extern const unsigned char KEY_ENTER;
extern const unsigned char KEY_SLASH;
extern const unsigned char KEY_BACKSPACE;
extern const unsigned char KEY_TAB;
extern const unsigned char KEY_DELETE;
extern const unsigned char KEY_SEMICOLON;
extern const unsigned char KEY_QUOTE;
extern const unsigned char KEY_PLUS;
extern const unsigned char KEY_MINUS;

class InputSystem
{
public:
	InputSystem();
	~InputSystem();
	void StartUp();
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	const XboxController &GetXboxController(int controllerID) const;
	//const ButtonKeyState &GetKeyState(unsigned char key);
private:
	XboxController _controllers[MAX_XBOX_CONTROLLERS] = {
		XboxController(0),
		XboxController(1),
		XboxController(2),
		XboxController(3),
	};
	//ButtonKeyState _keys[255];
};
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

const unsigned char KEY_ESC = VK_ESCAPE;
const unsigned char KEY_SPACE = VK_SPACE;
const unsigned char KEY_UPARROW = VK_UP;
const unsigned char KEY_LEFTARROW = VK_LEFT;
const unsigned char KEY_DOWNARROW = VK_DOWN;
const unsigned char KEY_RIGHTARROW = VK_RIGHT;
const unsigned char KEY_F1 = VK_F1;
const unsigned char KEY_F2 = VK_F2;
const unsigned char KEY_F3 = VK_F3;
const unsigned char KEY_F4 = VK_F4;
const unsigned char KEY_F6 = VK_F6;
const unsigned char KEY_F8 = VK_F8;
const unsigned char KEY_A = 'A';
const unsigned char KEY_D = 'D';
const unsigned char KEY_E = 'E';
const unsigned char KEY_N = 'N';
const unsigned char KEY_M = 'M';
const unsigned char KEY_R = 'R';
const unsigned char KEY_S = 'S';
const unsigned char KEY_W = 'W';
const unsigned char KEY_PERIOD = VK_OEM_PERIOD;
const unsigned char KEY_COMMA = VK_OEM_COMMA;
const unsigned char KEY_ENTER = VK_RETURN;
const unsigned char KEY_SLASH = VK_OEM_2; // /?
const unsigned char KEY_BACKSPACE = VK_BACK;
const unsigned char KEY_TAB = VK_TAB;
const unsigned char KEY_DELETE = VK_DELETE;
const unsigned char KEY_SEMICOLON = VK_OEM_1;
const unsigned char KEY_QUOTE = VK_OEM_7;
const unsigned char KEY_PLUS = VK_OEM_PLUS;
const unsigned char KEY_MINUS = VK_OEM_MINUS;
////////////////////////////////
InputSystem::InputSystem()
{
}

////////////////////////////////
InputSystem::~InputSystem()
{
}

////////////////////////////////
void InputSystem::StartUp()
{
	for (int i = 0; i < MAX_XBOX_CONTROLLERS; ++i) {
		_controllers[i].update();
	}
}

////////////////////////////////
void InputSystem::BeginFrame()
{
	for (int i = 0; i < MAX_XBOX_CONTROLLERS; ++i) {
		_controllers[i].update();
	}
}

////////////////////////////////
void InputSystem::EndFrame()
{

}

////////////////////////////////
void InputSystem::Shutdown()
{

}

////////////////////////////////
const XboxController &InputSystem::GetXboxController(int controllerID) const
{
	return _controllers[controllerID];
}

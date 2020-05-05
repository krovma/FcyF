#include "Engine/Input/ButtonKeyState.hpp"

////////////////////////////////
ButtonKeyState::ButtonKeyState()
{
	_thisFramePressed = false;
	_lastFramePressed = false;
}

////////////////////////////////
void ButtonKeyState::SetState(bool isPressed)
{
	_lastFramePressed = _thisFramePressed;
	_thisFramePressed = isPressed;
}

////////////////////////////////
void ButtonKeyState::Reset()
{
	_thisFramePressed = false;
	_lastFramePressed = false;
}

////////////////////////////////
bool ButtonKeyState::IsJustPressed() const
{
	return !_lastFramePressed && _thisFramePressed;
}

////////////////////////////////
bool ButtonKeyState::IsJustReleased() const
{
	return _lastFramePressed && !_thisFramePressed;
}

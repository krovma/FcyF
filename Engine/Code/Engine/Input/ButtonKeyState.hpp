#pragma once

//////////////////////////////////////////////////////////////////////////
class ButtonKeyState
{
public:
	ButtonKeyState();
	void SetState(bool isPressed);
	void Reset();
	// Getter
	bool IsPressed() const { return _thisFramePressed; }
	bool IsJustPressed() const;
	bool IsJustReleased() const;

protected:
	bool _thisFramePressed = false;
	bool _lastFramePressed = false;

};
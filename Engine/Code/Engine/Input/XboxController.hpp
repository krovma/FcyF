#pragma once
#include "Engine/Input/ButtonKeyState.hpp"
#include "Engine/Input/AnalogJoystick.hpp"
//////////////////////////////////////////////////////////////////////////
enum XboxButton
{
	XBOX_BUTTON_NOBUTTON	= -256,
	//////////////////////////////////////////////////////////////////////////
	XBOX_BUTTON_DPAD_UP		= 0,	
	XBOX_BUTTON_DPAD_DOWN,
	XBOX_BUTTON_DPAD_LEFT,
	XBOX_BUTTON_DPAD_RIGHT,
	XBOX_BUTTON_START,
	XBOX_BUTTON_BACK,
	XBOX_BUTTON_LSTICK,
	XBOX_BUTTON_RSTICK,
	XBOX_BUTTON_LB,
	XBOX_BUTTON_RB,
	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,
	//////////////////////////////////////////////////////////////////////////
	NUM_XBOX_BUTTONS
};
constexpr unsigned short XBOX_TRIGGER_MAX = 255u;
constexpr short XBOX_AXIS_MIN = -32768;
constexpr short XBOX_AXIS_MAX = 32767;

class XboxController
{
	friend class InputSystem;
public:
	explicit XboxController(int controllerID);

	void Reset();

	bool IsConnected() const { return _flagConnected; }
	int GetControllerID() const { return _controllerID; }
	const ButtonKeyState &GetButtonState(XboxButton button) const { return _buttonState[button]; }
	float GetLT() const { return _LTValue; }
	float GetRT() const { return _RTValue; }
	const AnalogJoystick &GetLeftJoystick() const { return _leftJoystick; }
	const AnalogJoystick &GetRightJoystick() const { return _rightJoystick; }

private:
	void update();
	void updateTrigger(float &triggerValue, unsigned char rawValue);
	void updateJoystick(AnalogJoystick &joystick, short rawX, short rawY);
	void updateButton(XboxButton button, unsigned short rawButtonFlag, unsigned short buttonMask);
private:
	const int _controllerID;
	bool _flagConnected;
	ButtonKeyState _buttonState[NUM_XBOX_BUTTONS];
	float _LTValue = 0.f;
	float _RTValue = 0.f;
	AnalogJoystick _leftJoystick = AnalogJoystick(XBOX_JOYSTICK_INNER_DEADZONE_FRACTION, XBOX_JOYSTICK_OUTTER_DEADZONE_FRACTION);
	AnalogJoystick _rightJoystick = AnalogJoystick(XBOX_JOYSTICK_INNER_DEADZONE_FRACTION, XBOX_JOYSTICK_OUTTER_DEADZONE_FRACTION);
};
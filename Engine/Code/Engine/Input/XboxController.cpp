#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/EngineCommon.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0")
////////////////////////////////
XboxController::XboxController(int controllerID)
	:_controllerID(controllerID)
{
	_flagConnected = true;
}

////////////////////////////////
void XboxController::Reset()
{
	_flagConnected = false;
	for (int i = 0; i < NUM_XBOX_BUTTONS; ++i)
		_buttonState[i].Reset();
	_LTValue = 0.f;
	_RTValue = 0.f;
	_leftJoystick.Reset();
	_rightJoystick.Reset();
}

////////////////////////////////
void XboxController::update()
{
	XINPUT_STATE controllerState;
	memset(&controllerState, 0, sizeof(XINPUT_STATE));
	DWORD errorStatus = XInputGetState(_controllerID, &controllerState);
	if (ERROR_SUCCESS == errorStatus) {
		_flagConnected = true;
		updateButton(XBOX_BUTTON_DPAD_UP, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
		updateButton(XBOX_BUTTON_DPAD_DOWN, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
		updateButton(XBOX_BUTTON_DPAD_LEFT, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
		updateButton(XBOX_BUTTON_DPAD_RIGHT, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
		updateButton(XBOX_BUTTON_A, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_A);
		updateButton(XBOX_BUTTON_B, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_B);
		updateButton(XBOX_BUTTON_X, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_X);
		updateButton(XBOX_BUTTON_Y, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_Y);
		updateButton(XBOX_BUTTON_START, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_START);
		updateButton(XBOX_BUTTON_BACK, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK);
		updateButton(XBOX_BUTTON_LB, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		updateButton(XBOX_BUTTON_RB, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		updateButton(XBOX_BUTTON_LSTICK, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
		updateButton(XBOX_BUTTON_RSTICK, controllerState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);

		updateTrigger(_LTValue, controllerState.Gamepad.bLeftTrigger);
		updateTrigger(_RTValue, controllerState.Gamepad.bRightTrigger);

		updateJoystick(_leftJoystick, controllerState.Gamepad.sThumbLX, controllerState.Gamepad.sThumbLY);
		updateJoystick(_rightJoystick, controllerState.Gamepad.sThumbRX, controllerState.Gamepad.sThumbRY);


	} else if (ERROR_DEVICE_NOT_CONNECTED == errorStatus) {
		Reset();
	} else {
		ERROR_RECOVERABLE("Conrtoller Error occured\n");
	}
}


////////////////////////////////
void XboxController::updateTrigger(float &triggerValue, unsigned char rawValue)
{
	triggerValue = (float)rawValue / (float)XBOX_TRIGGER_MAX;
}

////////////////////////////////
void XboxController::updateJoystick(AnalogJoystick &joystick, short rawX, short rawY)
{
	float normedX = (float)rawX / (float)XBOX_AXIS_MAX;
	float normedY = (float)rawY / (float)XBOX_AXIS_MAX;
	joystick.updatePosition(normedX, normedY);
}

////////////////////////////////
void XboxController::updateButton(XboxButton button, unsigned short rawButtonFlag, unsigned short buttonMask)
{
	unsigned short state = (rawButtonFlag & buttonMask);
	_buttonState[button].SetState(state != 0);
}

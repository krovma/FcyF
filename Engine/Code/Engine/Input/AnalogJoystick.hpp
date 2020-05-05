#pragma once
#include "Engine/Math/Vec2.hpp"

//////////////////////////////////////////////////////////////////////////
constexpr float XBOX_JOYSTICK_INNER_DEADZONE_FRACTION = 0.3f;
constexpr float XBOX_JOYSTICK_OUTTER_DEADZONE_FRACTION = 0.9f;

class AnalogJoystick
{
	friend class XboxController;
public:
	explicit AnalogJoystick(float innerDeadzoneFrac, float outterDeadzoneFrac);

	void Reset();
	float GetInnerDeadzoneFraction() const { return _innerDeadzoneFraction; }
	float GetOutterDeadzoneFraction() const { return _outterDeadzoneFraction; }

	Vec2 GetPosition() const { return _correctedPosition; }
	Vec2 GetRawPosition() const { return _rawPosition; }
	

	float GetJoystickAngleDegrees() const { return _correctedDegrees; }
	float GetMagnitude() const { return _correctedMagnitude; }

private:
	void updatePosition(float rawNormedX, float rawNormedY);

private:
	const float _innerDeadzoneFraction;
	const float _outterDeadzoneFraction;
	Vec2 _rawPosition = Vec2(0.f, 0.f);
	Vec2 _correctedPosition = Vec2(0.f, 0.f);
	float _correctedDegrees = 0.f;
	float _correctedMagnitude = 0.f;

};

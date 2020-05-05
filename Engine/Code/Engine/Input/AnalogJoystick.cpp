#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

////////////////////////////////
AnalogJoystick::AnalogJoystick(float innerDeadzoneFrac, float outterDeadzoneFrac)
	:_innerDeadzoneFraction(innerDeadzoneFrac)
	,_outterDeadzoneFraction(outterDeadzoneFrac)
{
}

////////////////////////////////
void AnalogJoystick::Reset()
{
	_correctedPosition = Vec2(0.f, 0.f);
	_rawPosition = Vec2(0.f, 0.f);
	_correctedMagnitude = 0.f;
	_correctedDegrees = 0.f;
}

////////////////////////////////
void AnalogJoystick::updatePosition(float rawNormedX, float rawNormedY)
{
	_rawPosition = Vec2(rawNormedX, rawNormedY);
	if (FloatGt(_rawPosition.GetLength(), _innerDeadzoneFraction)) {
		_correctedPosition = _rawPosition;
		if (FloatGt(_rawPosition.GetLength(), _outterDeadzoneFraction)) {
			_correctedPosition.SetLength(1.f);
		} else {
			_correctedPosition.SetLength(FloatMap(_rawPosition.GetLength(), _innerDeadzoneFraction, _outterDeadzoneFraction, 0.f, 1.f));
		}
		_correctedDegrees = _correctedPosition.GetAngleDegrees();
		_correctedMagnitude = _correctedPosition.GetLength();
	} else {
		_correctedPosition = Vec2(0, 0);
		_correctedMagnitude = 0.f;
		//_correctedDegrees = 0.f;
	}
}

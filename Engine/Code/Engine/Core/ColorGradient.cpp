#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ColorGradient.hpp"

STATIC ColorGradient ColorGradient::WHITE_NOGRADIENT(Rgba(1.f, 1.f, 1.f, 1.f));
STATIC ColorGradient ColorGradient::FADEOUT(Rgba(1.f, 1.f, 1.f, 1.f), Rgba(1.f, 1.f, 1.f, 0.f));

const Rgba ColorGradient::GetColorAt(float position /* = 0.5f */) const
{
	Rgba result;
	if (position <= 0.f) {
		return m_start;
	}
	if (position >= 1.f) {
		return m_end;
	}
	if (m_space == RGBA) {
		result = Lerp(m_start, m_end, position);
	} else if (m_space == HSL) {
		Vec3 start = m_start.ToHSL();
		Vec3 end = m_end.ToHSL();

		result = Rgba::FromHSL(Lerp(start, end, position));
		result.a = Lerp(m_start.a, m_end.a, position);
	}
	return result;
}

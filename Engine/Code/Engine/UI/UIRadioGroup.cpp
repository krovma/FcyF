#include "Engine/UI/UIRadioGroup.hpp"
#include "Engine/UI/UIWidget.hpp"

void UIRadioGroup::Activate(UIWidget* widget)
{
	for (auto each : m_group) {
		if (each == widget) {
			each->Select();
		} else {
			each->Unselect();
		}
	}
	m_current = widget;
}

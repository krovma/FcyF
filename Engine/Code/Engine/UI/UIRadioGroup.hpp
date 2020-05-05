#pragma once
#include <vector>

class UIWidget;

class UIRadioGroup
{
public:
	void Activate(UIWidget*);
	void AddWidget(UIWidget* widget) { m_group.push_back(widget);}
private:
	std::vector<UIWidget*> m_group;
	UIWidget* m_current = nullptr;
};
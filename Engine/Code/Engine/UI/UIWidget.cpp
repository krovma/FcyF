#include "Engine/UI/UIWidget.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/UI/UIRadioGroup.hpp"
STATIC AABB2 UIWidget::s_screenBounds;

UIWidget::UIWidget()
{
}

UIWidget::~UIWidget()
{
	delete m_mesh;
	m_mesh = nullptr;
	DestroyChildren();
}

void UIWidget::Render(RenderContext* renderer)
{
	RenderChildren(renderer);
}

UIWidget* UIWidget::AddChild(UIWidget* widget)
{
	if (widget->m_parent != this) {
		widget->UpdateBounds(m_worldBounds);
		widget->m_parent = this;
		m_children.push_back(widget);
		return widget;
	}
	return nullptr;
}

void UIWidget::RemoveChild(UIWidget* widget)
{
	if (widget == nullptr) {
		return;
	}
	for (size_t i = 0; i < m_children.size(); ++i) {
		if (m_children[i] == widget) {
			m_children[i] = m_children[m_children.size() - 1];
			m_children.pop_back();
			break;
		}
	}
	delete widget;
}

void UIWidget::SetRadioGroup(UIRadioGroup* radioGroup)
{
	m_radioGroup = radioGroup;
}

bool UIWidget::ResolveClick(const Vec2& cursor, bool clicking)
{
	for (auto each: m_children) {
		if (each->ResolveClick(cursor, clicking)) {
			return true;
		}
	}

	if (isInside(cursor)) {
		if (m_radioGroup != nullptr) {
			if (clicking) {
				m_radioGroup->Activate(this);
				FireEvent();
			}
		} else {
			//m_selected = true;
			if (clicking) {
				FireEvent();
			}
		}
		return true;
	}
	if (clicking) {
		m_selected = false;
	}
	return false;
}

bool UIWidget::isInside(const Vec2& pos)
{
	return m_worldBounds.IsPointInside(pos);
}

void UIWidget::FireEvent()
{
	return;
}

Vec2 UIWidget::GetWorldPosition() const
{
	return m_position;
}

AABB2 UIWidget::GetWorldBounds() const
{
	return m_worldBounds;
}

void UIWidget::UpdateBounds(const AABB2& bounds)
{
	const auto size = bounds.GetExtend() * 2.f;
	m_position = bounds.Min + Vec2(size.x * m_localPosition.x, size.y * m_localPosition.y) + m_localPositionOffset;
	const auto mySize = Vec2(size.x * m_localSizeRatio.x, size.y * m_localSizeRatio.y) + m_localSizeOffset;
	m_worldBounds.Min = m_position - Vec2(mySize.x * m_pivot.x, mySize.y * m_pivot.y);
	m_worldBounds.Max = m_worldBounds.Min + mySize;

	UpdateChildrenBounds();
}

void UIWidget::SetLocalPosition(const Vec2& position, const Vec2& offset)
{
	m_localPosition = position;
	m_localPositionOffset = offset;
	if (m_parent != nullptr) {
		UpdateBounds(m_parent->m_worldBounds);
	} else {
		UpdateBounds(s_screenBounds);
	}
}

void UIWidget::SetLocalSize(const Vec2& size, const Vec2& offset)
{
	m_localSizeRatio = size;
	m_localSizeOffset = offset;
	if (m_parent != nullptr) {
		UpdateBounds(m_parent->m_worldBounds);
	} else {
		UpdateBounds(s_screenBounds);
	}
}

void UIWidget::SetPivot(const Vec2& pivot)
{
	m_pivot = pivot;
	if (m_parent != nullptr) {
		UpdateBounds(m_parent->m_worldBounds);
	} else {
		UpdateBounds(s_screenBounds);
	}
}

void UIWidget::Select()
{
	m_selected = true;
}

void UIWidget::Unselect()
{
	m_selected = false;
}

void UIWidget::UpdateChildrenBounds()
{
	for (auto each:m_children) {
		each->UpdateBounds(m_worldBounds);
	}
}

void UIWidget::RenderChildren(RenderContext* renderer) const
{
	for (auto each : m_children) {
		each->Render(renderer);
	}
}

void UIWidget::DestroyChildren()
{
	for (auto each : m_children) {
		delete each;
	}
	m_children.clear();
}

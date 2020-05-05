#pragma once
#include "Engine/Math/AABB2.hpp"
#include <vector>
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

class TextureView2D;
class UIRadioGroup;
class RenderContext;

class UIWidget
{
public:
	static AABB2 s_screenBounds;
public:
	UIWidget();
	virtual ~UIWidget();

	virtual void Render(RenderContext* renderer);

	UIWidget* AddChild(UIWidget* widget);
	void RemoveChild(UIWidget* widget);
	void SetRadioGroup(UIRadioGroup* radioGroup);
	virtual bool ResolveClick(const Vec2& cursor, bool click = false);
	virtual bool isInside(const Vec2& pos);
	virtual void FireEvent();
	Vec2 GetWorldPosition() const;
	AABB2 GetWorldBounds() const;

	void UpdateBounds(const AABB2& bounds);
	void SetLocalPosition(const Vec2& position, const Vec2& offset = Vec2::ZERO);
	void SetLocalSize(const Vec2& size, const Vec2& offset = Vec2::ZERO);
	void SetPivot(const Vec2& pivot);

	void Select();
	void Unselect();

protected:
	void UpdateChildrenBounds();
	void RenderChildren(RenderContext* renderer) const;
	void DestroyChildren();

protected:
	GPUMesh* m_mesh = nullptr;
	bool m_selected = false;
	UIWidget* m_parent = nullptr;
	std::vector<UIWidget*> m_children;

	Vec2 m_localPosition = Vec2(.5f, .5f);
	Vec2 m_localPositionOffset = Vec2::ZERO;
	Vec2 m_localSizeRatio = Vec2::ONE;
	Vec2 m_localSizeOffset = Vec2::ZERO;
	Vec2 m_pivot = Vec2(.5f, .5f);

	Vec2 m_position;
	AABB2 m_worldBounds;
	UIRadioGroup* m_radioGroup = nullptr;

public:
	Vec2 GetLocalPosition() const
	{
		return m_localPosition;
	}

	Vec2 GetLocalPositionOffset() const
	{
		return m_localPositionOffset;
	}

	Vec2 GetLocalSizeRatio() const
	{
		return m_localSizeRatio;
	}

	Vec2 GetLocalSizeOffset() const
	{
		return m_localSizeOffset;
	}

	Vec2 GetPivot() const
	{
		return m_pivot;
	}

};

class UICanvas : public UIWidget
{
public:
	virtual ~UICanvas() override = default;
	virtual void Render(RenderContext* renderer) override
	{
		UIWidget::Render(renderer);
	}

	virtual bool ResolveClick(const Vec2& cursor, bool clicking) override
	{
		for (auto each : m_children) {
			if (each->ResolveClick(cursor, clicking)) {
				return true;
			}
		}
		return false;
	}
};

class UILabel : public UIWidget
{
public:
	UILabel() = default;
	UILabel(BitmapFont* font);
	virtual ~UILabel() override;
	virtual void Render(RenderContext* renderer) override;

	virtual bool isInside(const Vec2& pos) override;
protected:
	BitmapFont* m_font = nullptr;
	std::string m_text;
	float m_textSize = 20.f;
	Rgba m_tint = Rgba::WHITE;
	Vec2 m_alignment = BitmapFont::ALIGNMENT_CENTER;
public:
	BitmapFont* GetFont() const;
	void SetFont(BitmapFont* const font);
	std::string GetText() const;
	void SetText(const std::string& text);
	float GetTextSize() const;
	void SetTextSize(const float textSize);
	Vec2 GetAlignment() const;
	void SetAlignment(const Vec2& alignment);
	Rgba GetTint() const;
	void SetTint(const Rgba& tint);
};

class UIButton : public UIWidget
{
public:
	UIButton() {};
	UIButton(const std::string& caption, BitmapFont* font);
	virtual ~UIButton() override;
	virtual void Render(RenderContext* renderer) override;
	std::string EventString = "";

	virtual void FireEvent() override;
protected:
	TextureView2D* m_texture = nullptr;
	UILabel* m_label = nullptr;
	Rgba m_normalTint = Rgba::WHITE;
	Rgba m_activateTint = Rgba::RED;
public:
	TextureView2D* GetTexture() const;
	void SetTexture(TextureView2D* const texture);
	UILabel* GetButtonLabel() const;
};
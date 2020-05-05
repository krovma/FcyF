#include "Engine/UI/UIWidget.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

UILabel::UILabel(BitmapFont* font)
	:m_font(font)
{
}

UILabel::~UILabel()
{
}

void UILabel::Render(RenderContext* renderer)
{
	if (m_mesh == nullptr) {
		m_mesh = new GPUMesh(renderer);
		m_mesh->SetLayout(RenderBufferLayout::AcquireLayoutFor<Vertex_PCU>());
	}
	std::vector<Vertex_PCU> verts;
	m_font->AddTextInBox(verts, m_text, GetWorldBounds(), m_alignment, m_textSize, m_tint);
	m_mesh->CopyVertexPCUArray(verts.data(), verts.size());

	renderer->BindTextureViewWithSampler(TEXTURE_SLOT_DIFFUSE, m_font->GetFontTexture());
	renderer->DrawMesh(*m_mesh);
	UIWidget::Render(renderer);
}

bool UILabel::isInside(const Vec2& pos)
{
	UNUSED(pos);
	return false;
}


BitmapFont* UILabel::GetFont() const
{
	return m_font;
}

void UILabel::SetFont(BitmapFont* const font)
{
	m_font = font;
}

std::string UILabel::GetText() const
{
	return m_text;
}

void UILabel::SetText(const std::string& text)
{
	m_text = text;
}

float UILabel::GetTextSize() const
{
	return m_textSize;
}

void UILabel::SetTextSize(const float textSize)
{
	m_textSize = textSize;
}

Vec2 UILabel::GetAlignment() const
{
	return m_alignment;
}

void UILabel::SetAlignment(const Vec2& alignment)
{
	m_alignment = alignment;
}

Rgba UILabel::GetTint() const
{
	return m_tint;
}

void UILabel::SetTint(const Rgba& tint)
{
	m_tint = tint;
}

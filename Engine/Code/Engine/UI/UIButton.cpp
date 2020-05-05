#include "Engine/UI/UIWidget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Event/EventSystem.hpp"

UIButton::UIButton(const std::string& caption, BitmapFont* font)
{
	m_label = new UILabel(font);
	m_label->SetText(caption);

	AddChild(m_label);
}

UIButton::~UIButton()
{
}

void UIButton::Render(RenderContext* renderer)
{
	if (m_mesh == nullptr) {
		m_mesh = new GPUMesh(renderer);
		m_mesh->SetLayout(RenderBufferLayout::AcquireLayoutFor<Vertex_PCU>());
	}
	std::vector<Vertex_PCU> verts;
	AddVerticesOfAABB2D(verts, GetWorldBounds(), m_selected ? m_activateTint : m_normalTint);
	m_mesh->CopyVertexPCUArray(verts.data(), verts.size());

	renderer->BindTextureViewWithSampler(TEXTURE_SLOT_DIFFUSE, m_texture);

	renderer->DrawMesh(*m_mesh);
	UIWidget::Render(renderer);
}

void UIButton::FireEvent()
{
	g_Event->Trigger(EventString);
}

TextureView2D* UIButton::GetTexture() const
{
	return m_texture;
}

void UIButton::SetTexture(TextureView2D* const texture)
{
	m_texture = texture;
}

UILabel* UIButton::GetButtonLabel() const
{
	return m_label;
}

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/RenderTargetView.hpp"

////////////////////////////////
RenderTargetView::~RenderTargetView()
{
	DX_SAFE_RELEASE(m_renderTargetView);
}

////////////////////////////////
float RenderTargetView::GetHeight() const
{
	return m_size.y;
}

////////////////////////////////
float RenderTargetView::GetWidth() const
{
	return m_size.x;
}

////////////////////////////////
void RenderTargetView::_CreateFromInternalTexture(ID3D11Device* device, ID3D11Texture2D* texture)
{
	DX_SAFE_RELEASE(m_renderTargetView);
	
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);
	m_size.x = (float)desc.Width;
	m_size.y = (float)desc.Height;
	
	HRESULT hr = device->CreateRenderTargetView(texture, nullptr, &m_renderTargetView);
	GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to create render target view\n");
}

/*
void RenderTargetView::_CreateFromDescribtion(ID3D11Device* device, D3D11_TEXTURE2D_DESC* desc)
{
	DX_SAFE_RELEASE(m_renderTargetView);
	m_size.x = (float)(desc->Width);
	m_size.y = (float)(desc->Height);
	HRESULT hr = device->CreateRenderTargetView((ID3D11Resource*)desc, nullptr, &m_renderTargetView);
	GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to create render target view\n");
}
*/
#pragma once
#include "Engine/Math/Vec2.hpp"
//////////////////////////////////////////////////////////////////////////
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11Device;
struct D3D11_TEXTURE2D_DESC;
//////////////////////////////////////////////////////////////////////////
class RenderTargetView
{
public:
	friend class RenderContext;
	RenderTargetView() {/*Do Nothing*/}
	~RenderTargetView();

	float GetHeight() const;
	float GetWidth() const;
	float GetAspect() const { return GetWidth() / GetHeight(); }
	ID3D11RenderTargetView* GetDXResource() const { return m_renderTargetView; }
private:
	void _CreateFromInternalTexture(ID3D11Device* device, ID3D11Texture2D* texture);
	//void _CreateFromDescribtion(ID3D11Device* device, D3D11_TEXTURE2D_DESC* desc);
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	Vec2 m_size = Vec2::ZERO;
};
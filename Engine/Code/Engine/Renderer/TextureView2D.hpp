#pragma once
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Math/IntVec2.hpp"
//////////////////////////////////////////////////////////////////////////
class Sampler;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;
//////////////////////////////////////////////////////////////////////////
class TextureView
{
public:
	TextureView();
	~TextureView();

	void SetSampler(Sampler* sampler) { m_sampler = sampler; }
	Sampler* GetSampler() const { return m_sampler; }
	ID3D11ShaderResourceView* GetView() const { return m_view; }
protected:
	ID3D11ShaderResourceView* m_view = nullptr;
	ID3D11Resource* m_resource = nullptr;
	Sampler* m_sampler;
};
//////////////////////////////////////////////////////////////////////////
class TextureView2D : public TextureView
{
public:
	friend class Texture2D;
	IntVec2 m_size;
};
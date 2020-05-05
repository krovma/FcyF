#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
//////////////////////////////////////////////////////////////////////////
static D3D11_FILTER GetD3DFilter(FilterMode min, FilterMode mag)
{
	static D3D11_FILTER filters[NUM_FILTER_MODES][NUM_FILTER_MODES] = {
		/*min/mag*//*POINT*//*LINEAR*/
		/*POINT*/{D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,},
		/*LINEAR*/{D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT, D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,},
	};
	return filters[min][mag];
}
//////////////////////////////////////////////////////////////////////////

////////////////////////////////
Sampler::Sampler()
{

}

////////////////////////////////
Sampler::~Sampler()
{
	DX_SAFE_RELEASE(m_handle);
}

////////////////////////////////
void Sampler::CreateState(const RenderContext* renderer)
{
	if (!m_dirty) {
		return;
	}
	DX_SAFE_RELEASE(m_handle);
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Filter = GetD3DFilter(m_minFilter, m_magFilter);
	desc.MaxAnisotropy = 1u;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	desc.MipLODBias = 0.f;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr = renderer->GetDevice()->CreateSamplerState(&desc, &m_handle);
	GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to create sampler state");
	m_dirty = false;
}


#pragma once
//////////////////////////////////////////////////////////////////////////
class RenderContext;
struct ID3D11SamplerState;
//////////////////////////////////////////////////////////////////////////
#include "Engine/Renderer/RenderTypes.hpp"
//////////////////////////////////////////////////////////////////////////
class Sampler
{
public:
	Sampler();
	~Sampler();

	void CreateState(const RenderContext* renderer);
	void SetFilterModes(FilterMode min, FilterMode mag) { m_minFilter = min; m_magFilter = mag; }
	ID3D11SamplerState* GetSampleState() const { return m_handle; }

private:
	ID3D11SamplerState* m_handle = nullptr;
	FilterMode m_minFilter;
	FilterMode m_magFilter;
	bool m_dirty = true;
};
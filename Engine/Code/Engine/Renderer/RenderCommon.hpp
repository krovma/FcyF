#pragma once
#include <d3d11.h>  
#include <DXGI.h>    

// D3D DEBUG 
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define DX_SAFE_RELEASE(dx_resource)\
 if ((dx_resource) != nullptr) {\
	dx_resource->Release();\
	dx_resource = nullptr;\
 }
#include "Engine/Renderer/RenderTypes.hpp"
D3D11_USAGE GetD3DUsageFromGPUMemoryUsage(GPUMemoryUsage usage);
unsigned int GetD3DBufferUsageFromRenderBufferUsage(RenderBufferUsage usage);
unsigned int GetD3DBind(unsigned int usage);

//////////////////////////////////////////////////////////////////////////
D3D11_COMPARISON_FUNC GetD3DComparisonFunc(CompareOperator op);
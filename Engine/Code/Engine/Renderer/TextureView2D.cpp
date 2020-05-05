#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
//////////////////////////////////////////////////////////////////////////

////////////////////////////////
TextureView::TextureView()
{

}

////////////////////////////////
TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_view);
	DX_SAFE_RELEASE(m_resource);
}

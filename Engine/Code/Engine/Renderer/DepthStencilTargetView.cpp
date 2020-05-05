#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Renderer/RenderCommon.hpp"

////////////////////////////////
DepthStencilTargetView::DepthStencilTargetView(const IntVec2& size)
	:m_size(size)
{

}

////////////////////////////////
DepthStencilTargetView::~DepthStencilTargetView()
{
	DX_SAFE_RELEASE(m_handle);
	DX_SAFE_RELEASE(m_view);
}

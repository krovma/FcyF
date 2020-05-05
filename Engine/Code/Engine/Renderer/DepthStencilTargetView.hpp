#pragma once
#include "Engine/Math/IntVec2.hpp"
//////////////////////////////////////////////////////////////////////////
struct ID3D11Resource;
struct ID3D11DepthStencilView;
//////////////////////////////////////////////////////////////////////////

class DepthStencilTargetView
{
public:
	DepthStencilTargetView(const IntVec2& size);
	~DepthStencilTargetView();

	int GetWidth() const { return m_size.x; }
	int GetHeight() const { return m_size.y;  }
	ID3D11Resource* GetHandle() const { return m_handle; }
	ID3D11DepthStencilView* GetView() const { return m_view; }
	void SetHandle(ID3D11Resource* handle) { m_handle = handle; }
	void SetView(ID3D11DepthStencilView* view) { m_view = view; }
private:
	ID3D11Resource* m_handle = nullptr;
	ID3D11DepthStencilView* m_view = nullptr;
	IntVec2 m_size;
};
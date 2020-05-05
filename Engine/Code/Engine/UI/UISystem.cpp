#include "Engine/UI/UISystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderTargetView.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_win32.h"
#include "ThirdParty/imgui/imgui_impl_dx11.h"
#include "Engine/Renderer/RenderCommon.hpp"

//////////////////////////////////////////////////////////////////////////
UISystem* g_theUI = nullptr;
////////////////////////////////
void UISystem_Imgui::Startup(WindowContext* window, RenderContext* renderer)
{
	m_renderer = renderer;
	m_window = window;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_window->m_hWnd);
	ImGui_ImplDX11_Init(m_renderer->GetDevice(), m_renderer->GetContext());
	ImGui::StyleColorsLight();
}

////////////////////////////////
void UISystem_Imgui::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

////////////////////////////////
void UISystem_Imgui::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

////////////////////////////////
void UISystem_Imgui::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

////////////////////////////////
void UISystem_Imgui::Render() const
{
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	RenderTargetView* rtv = m_renderer->GetFrameColorTarget();
	ID3D11RenderTargetView* dxrtv = rtv->GetDXResource();

	m_renderer->GetContext()->OMSetRenderTargets(1, &dxrtv, nullptr);

	ImGui_ImplDX11_RenderDrawData(drawData);
}

////////////////////////////////
void UISystem_Imgui::EndFrame()
{
	ImGui::EndFrame();
}

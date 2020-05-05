#include "Game/EngineBuildPreferences.hpp"
#include "Material.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Core/Image.hpp"
#include "ThirdParty/stb/stb_image.h"
#include <cstring>
#include <vector>
#include "Engine/Develop/Profile.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Core/Job.hpp"
#include "Engine/Develop/Log.hpp"
#define RENDER_DEBUG_LEAK
#define RENDER_DEBUG_REPORT

//#include <d3dx11.h>
////////////////////////////////
RenderContext::RenderContext(void* hWnd, unsigned int resWidth, unsigned int resHeight)
{
	m_resolution.x = (int)resWidth;
	m_resolution.y = (int)resHeight;
//Creating d3d rendering context
	UINT device_flags = 0U;
#if defined(RENDER_DEBUG_LEAK)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

//Swap chain
	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset(&swap_desc, 0, sizeof(swap_desc));

	// fill the swap chain description struct
	swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	swap_desc.OutputWindow = (HWND)hWnd;                                // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)
																  // Default options.
	swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	swap_desc.BufferDesc.Width = resWidth;
	swap_desc.BufferDesc.Height = resHeight;


	// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,  // Driver Type - We want to use the GPU (HARDWARE)
		nullptr,                   // Software Module - DLL that implements software mode (we do not use)
		device_flags,              // device creation options
		nullptr,                   // feature level (use default)
		0U,                        // number of feature levels to attempt
		D3D11_SDK_VERSION,         // SDK Version to use
		&swap_desc,                // Description of our swap chain
		&m_swapChain,            // Swap Chain we're creating
		&m_device,               // [out] The device created
		nullptr,                   // [out] Feature Level Acquired
		&m_context);            // Context that can issue commands on this pipe.

								  // SUCCEEDED & FAILED are macros provided by Windows to checking
								  // the results.  Almost every D3D call will return one - be sure to check it.
	GUARANTEE_OR_DIE(SUCCEEDED(hr), "Failed to create D3D render context\n");

	m_immediateVBO = new VertexBuffer(this);
	m_immediateVBO->SetLayout(RenderBufferLayout::AcquireLayoutFor<Vertex_PCU>());
	m_modelBuffer = new ConstantBuffer(this);
	m_lightBuffer = new ConstantBuffer(this);
	m_lightBuffer->Buffer(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));
	m_postBuffer = new ConstantBuffer(this);
#if defined(RENDER_DEBUG_REPORT)
	hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debug));
	if (SUCCEEDED(hr)) {
		DebuggerPrintf("Debug context created!!!!!!\n");
	}
#endif
}

//////////////////////////////////////////////////////////////////////////
void RenderContext::Startup()
{
	Sampler* point = new Sampler();
	point->SetFilterModes(FILTER_MODE_POINT, FILTER_MODE_POINT);
	m_cachedSamplers[SAMPLER_POINT] = point;
	Sampler* linear = new Sampler();
	linear->SetFilterModes(FILTER_MODE_LINEAR, FILTER_MODE_LINEAR);
	m_cachedSamplers[SAMPLER_LINEAR] = linear;

	Image* whitepx = new Image(1, 1, "White");
	whitepx->SetTexelColor(0, 0, Rgba::WHITE);
	Texture2D* whiteTexture = new Texture2D(this);
	whiteTexture->LoadFromImage(whitepx);
	m_LoadedTexture["White"] = whiteTexture;
	m_cachedTextureView[whiteTexture] = whiteTexture->CreateTextureView();
	delete whitepx;
	Image* flatpx = new Image(1, 1, "Flat");
	flatpx->SetTexelColor(0, 0, Rgba::FLAT);
	Texture2D* flatTexture = new Texture2D(this);
	flatTexture->LoadFromImage(flatpx);
	m_LoadedTexture["Flat"] = flatTexture;
	m_cachedTextureView[flatTexture] = flatTexture->CreateTextureView();
	delete flatpx;
	Image* blackpx = new Image(1, 1, "Black");
	blackpx->SetTexelColor(0, 0, Rgba::BLACK);
	Texture2D* blackTexture = new Texture2D(this);
	blackTexture->LoadFromImage(blackpx);
	m_LoadedTexture["Black"] = blackTexture;
	m_cachedTextureView[blackTexture] = blackTexture->CreateTextureView();
	delete blackpx;

	m_defaultDepthStencilTexture = Texture2D::CreateDepthStencilTarget(this, m_resolution.x, m_resolution.y);
	m_defaultDepthSencilTargetView = m_defaultDepthStencilTexture->CreateDepthStencilTargetView();

	if (g_defaultMaterial == nullptr) {
		g_defaultMaterial = new Material(this);
		for (unsigned int slot = 0u; slot < NUM_USED_TEXTURES; ++slot) {
			g_defaultMaterial->SetTexture(slot, static_cast<Texture2D*>(nullptr));
		}
	}

	m_effectCamera = new Camera(Vec2::ZERO, Vec2(m_resolution));
}

void RenderContext::BeginFrame()
{
	ID3D11Texture2D* pBackBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	m_backBuffer = Texture2D::WrapD3DTexture(this, pBackBuffer);
	m_frameTexture = new Texture2D(this, pBackBuffer);
	m_frameRenderTarget = new RenderTargetView();
	m_frameRenderTarget->_CreateFromInternalTexture(m_device, m_frameTexture->GetHandle());
	DX_SAFE_RELEASE(pBackBuffer); //release my hold on it (does not destroy it!)
}

void RenderContext::EndFrame()
{
	CopyTexture(m_backBuffer, m_frameTexture);
	m_swapChain->Present(0, // Sync Interval, set to 1 for VSync
		0);                    // Present flags, see;
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
	delete m_frameRenderTarget;
	delete m_frameTexture;
	m_frameRenderTarget = nullptr;
	delete m_backBuffer;
	// #SD2ToDo: just update the m_rtv (being sure to release the old one)
}

void RenderContext::Shutdown()
{
	for (const auto& eachShader : m_LoadedShader) {
		delete eachShader.second;
	}
	m_LoadedShader.clear();
	for (const auto& eachSampler : m_cachedSamplers) {
		delete eachSampler;
	}
	for (const auto& eachTexture : m_LoadedTexture) {
		delete eachTexture.second;
	}
	m_LoadedTexture.clear();
	for (const auto& eachTextureView : m_cachedTextureView) {
		delete eachTextureView.second;
	}
	m_cachedTextureView.clear();
	for (const auto& eachModel: m_LoadedMesh) {
		delete eachModel.second;
	}
	m_LoadedMesh.clear();

	delete m_immediateVBO;
	delete m_immediateMesh;
	delete m_modelBuffer;
	delete m_lightBuffer;
	delete m_defaultDepthStencilTexture;
	delete m_defaultDepthSencilTargetView;
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_context);
	DX_SAFE_RELEASE(m_device);
#if defined(RENDER_DEBUG_REPORT)
	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	DX_SAFE_RELEASE(m_debug);
#endif
}

////////////////////////////////
RenderTargetView* RenderContext::GetFrameColorTarget() const
{
	return m_frameRenderTarget;
}

RenderTargetView* RenderContext::GetNewRenderTarget(D3D11_TEXTURE2D_DESC* desc)
{
	UNUSED(desc);
	return nullptr;
}

RenderTargetView* RenderContext::GetNewRenderTarget(Texture2D* targetTexture)
{
	RenderTargetView *newRtv = new RenderTargetView();
	newRtv->_CreateFromInternalTexture(m_device, targetTexture->GetHandle());
	return newRtv;
}

Texture2D* RenderContext::GetNewScratchTextureLike(Texture2D* reference)
{
	return new Texture2D(this, reference->GetHandle());
}

////////////////////////////////
DepthStencilTargetView* RenderContext::GetFrameDepthStencilTarget() const
{
	return m_defaultDepthSencilTargetView;
}

void RenderContext::ClearColorTarget(const Rgba &clearColor) const
{
	m_context->ClearRenderTargetView(m_frameRenderTarget->m_renderTargetView, (const FLOAT *)&clearColor);
}

////////////////////////////////
void RenderContext::ClearDepthStencilTarget(float depth /*= 1.0f*/, unsigned char stencil /*= 0u*/)
{
	ID3D11DepthStencilView* dsv = nullptr;
	dsv = m_currentCamera->GetDepthStencilTargetView()->GetView();
	m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
}

void RenderContext::CopyTexture(Texture2D* dst, Texture2D* src)
{
	if (dst->IsImmutable()) {
		ERROR_RECOVERABLE("Cannot copy to immutable texture\n");
		return;
	}
	
	m_context->CopyResource(dst->GetHandle(), src->GetHandle());
}

void RenderContext::ApplyEffect(RenderTargetView* dst, TextureView2D* src, Material* material)
{
	Camera* lastCamera = m_currentCamera;
	Shader* lastShader = m_currentShader;

	Texture2D *copyDepth = new Texture2D(this, m_defaultDepthStencilTexture->GetHandle());
	CopyTexture(copyDepth, m_defaultDepthStencilTexture);
	TextureView2D* viewDepth = copyDepth->CreateTextureView();

	EndCamera(*m_currentCamera);
	m_effectCamera->SetRenderTarget(dst);
	BeginCamera(*m_effectCamera);
	material->UseMaterial(this);
	BindConstantBuffer(CONSTANT_SLOT_POSTPROCESS, m_postBuffer);
	BindTextureView(TEXTURE_SLOT_DIFFUSE, src);
	BindTextureViewWithSampler(2, viewDepth);
	Draw(3);
	BindTextureView(TEXTURE_SLOT_DIFFUSE, nullptr);
	EndCamera(*m_effectCamera);
	BeginCamera(*lastCamera);
	BindShader(lastShader);

	delete viewDepth;
	delete copyDepth;
}

////////////////////////////////
void RenderContext::BindShader(Shader* shader)
{
	m_currentShader = shader;

	m_context->VSSetShader(shader->GetVertexShader(), nullptr, 0u);
	m_context->PSSetShader(shader->GetPixelShader(), nullptr, 0u);
}

////////////////////////////////
void RenderContext::BindConstantBuffer(ConstantBufferSlot slot, ConstantBuffer* buffer)
{
	ID3D11Buffer *buf = (buffer != nullptr) ? (buffer->GetHandle()) : nullptr;
	m_context->VSSetConstantBuffers(slot, 1u, &buf);
	m_context->PSSetConstantBuffers(slot, 1u, &buf);
}

////////////////////////////////
void RenderContext::BindVertexBuffer(VertexBuffer* buffer) const
{
	unsigned int stride = buffer->GetLayout()->GetStride();
	unsigned int offset = 0;
	ID3D11Buffer* buf = buffer->GetHandle();
	m_context->IASetVertexBuffers(0, 1, &buf, &stride, &offset);
}

////////////////////////////////
void RenderContext::BindIndexBuffer(IndexBuffer* buffer) const
{
	ID3D11Buffer *handle = nullptr;
	if (buffer != nullptr) {
		handle = buffer->GetHandle();
	}
	m_context->IASetIndexBuffer(handle, DXGI_FORMAT_R32_UINT, 0);
}

void RenderContext::BeginCamera(Camera &camera)
{
	// Set Render target
	m_currentCamera = &camera;
	RenderTargetView* renderTarget = m_currentCamera->GetRenderTarget();
	//#SD2ToDo: If view is nullptr, use the frame's backbuffer; 
	DepthStencilTargetView* dsv = camera.GetDepthStencilTargetView();
	ID3D11DepthStencilView* dsvView = nullptr;
	if (dsv != nullptr) {
		dsvView = dsv->GetView();
	}
	m_context->OMSetRenderTargets(1u, &(renderTarget->m_renderTargetView), dsvView);
	// Set Viewport
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.TopLeftX = 0u;
	viewport.TopLeftY = 0u;
	viewport.Width = renderTarget->GetWidth();
	viewport.Height = renderTarget->GetHeight();
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_context->RSSetViewports(1, &viewport);
	m_currentCamera->UpdateConstantBuffer(this);
	BindConstantBuffer(CONSTANT_SLOT_CAMERA, m_currentCamera->GetConstantBuffer());
	BindConstantBuffer(CONSTANT_SLOT_LIGHT, m_lightBuffer);
}

void RenderContext::EndCamera(Camera &camera)
{
	UNUSED(camera);
	m_context->OMSetRenderTargets(0u, nullptr, nullptr);
	m_currentCamera = nullptr;
}

////////////////////////////////
void RenderContext::SetBlendMode(BlendMode mode)
{
	m_blendMode = mode;
	_loadBlendFunc();
}

////////////////////////////////
void RenderContext::Draw(int vertexCount, unsigned int byteOffset/*=0u*/) const
{
	PROFILE_SCOPE(__FUNCTION__);
	m_currentShader->UpdateShaderStates(this);
	static float black[] = { 0.f,0.f,0.f,1.f };
	m_context->OMSetBlendState(m_currentShader->GetBlendState(), black, 0xffffffff);
	m_context->OMSetDepthStencilState(m_currentShader->GetDepthStencilState(), 0);
	m_context->RSSetState(m_currentShader->GetRasterizerState());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_currentShader->GetVertexBufferLayout());
	m_context->Draw((UINT)vertexCount, byteOffset);
}

////////////////////////////////
void RenderContext::DrawIndexed(int count) 
{
	PROFILE_SCOPE(__FUNCTION__);
	m_currentShader->UpdateShaderStates(this);
	static float black[] = { 0.f,0.f,0.f,1.f };
	m_context->OMSetBlendState(m_currentShader->GetBlendState(), black, 0xffffffff);
	m_context->OMSetDepthStencilState(m_currentShader->GetDepthStencilState(), 0);
	m_context->RSSetState(m_currentShader->GetRasterizerState());
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_currentShader->GetVertexBufferLayout());
	m_context->DrawIndexed(count, 0, 0);
}

void RenderContext::DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const
{
	m_immediateVBO->Buffer(vertices, numVertices);
	bool result = m_currentShader->CreateVertexPCULayout(this);
	GUARANTEE_OR_DIE(result, "Can not crate input layout\n");
	BindVertexBuffer(m_immediateVBO);
	Draw(numVertices);
}

////////////////////////////////
void RenderContext::DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const
{
	DrawVertexArray((int)numVertices, vertices.data());
}

////////////////////////////////
void RenderContext::DrawMesh(const GPUMesh& mesh)
{
	BindVertexBuffer(mesh.GetVertexBuffer());
	BindIndexBuffer(mesh.GetIndexBuffer());
	bool result = m_currentShader->CreateVertexBufferLayout(this, mesh.GetLayout());
	GUARANTEE_OR_DIE(result, "Can not crate input layout\n");

	if (m_lightDirty) {
		m_lightDirty = false;
		m_lightBuffer->Buffer(&m_cpuLightBuffer, sizeof(m_cpuLightBuffer));
	}

	if (mesh.IsUsingIndexBuffer()) {
		DrawIndexed(mesh.GetElementCount());
	} else {
		Draw(mesh.GetElementCount());
	}
}

////////////////////////////////
void RenderContext::DrawModel(const Model& model)
{
	model.GetMaterial()->UseMaterial(this);
	DrawMesh(*model.GetMesh());
}

static void WriteImageCB(Job* job);
class WriteImageJob : public Job
{
public:
	std::string m_path;
	ID3D11Texture2D* m_texture = nullptr;
	D3D11_MAPPED_SUBRESOURCE* m_mapped = nullptr;
	int width;
	int height;
	WriteImageJob(const std::string& path, int width, int height, ID3D11Texture2D* texture, D3D11_MAPPED_SUBRESOURCE* mapped)
		: m_path(path),
		  width(width),
		  height(height)
	{
		m_texture = texture;
		m_mapped = mapped;
		SetFinishCallback(WriteImageCB);
	}

	void Run() override
	{
		Image* tempImg = new Image(width, height, m_path.c_str());
		unsigned int rowpitch = m_mapped->RowPitch;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				
				tempImg->SetTexelColor(x, y, Rgba(
					((unsigned char*)m_mapped->pData)[y * rowpitch + x * 4 + 0],
					((unsigned char*)m_mapped->pData)[y * rowpitch + x * 4 + 1],
					((unsigned char*)m_mapped->pData)[y * rowpitch + x * 4 + 2],
					((unsigned char*)m_mapped->pData)[y * rowpitch + x * 4 + 3]
				));

			}
		}
		tempImg->WriteFile();
	}

};

static void WriteImageCB(Job* job)
{
	WriteImageJob* j = (WriteImageJob*)job;
	g_theRenderer->GetContext()->Unmap(j->m_texture, 0);
	delete j->m_mapped;
	j->m_texture->Release();
	Log("Game", "Screenshot saved to %s", j->m_path.c_str());
}
////////////////////////////////
void RenderContext::Screenshoot(const std::string& path)
{
	D3D11_TEXTURE2D_DESC desc;
	ID3D11Texture2D* backbuffer = GetFrameTexture()->GetHandle();
	//m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& backbuffer);
	backbuffer->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	ID3D11Texture2D* backbufferStaged = nullptr;
	m_device->CreateTexture2D(&desc, nullptr, &backbufferStaged);
	m_context->CopyResource(backbufferStaged, backbuffer);

	D3D11_MAPPED_SUBRESOURCE* resource = new D3D11_MAPPED_SUBRESOURCE();
	HRESULT hr = m_context->Map(backbufferStaged, 0, D3D11_MAP_READ, 0, resource);
	
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Screenshot create resource failed");
	}

	auto job = new WriteImageJob(path, desc.Width, desc.Height, backbufferStaged, resource);
	job->SetCatagory(JOB_GENERIC);
	g_theJobSystem->Run(job);
	//pBackBufferStaging->Release();
	//backbuffer->Release();
}

////////////////////////////////
Texture2D* RenderContext::AcquireTextureFromFile(const char* imageFilePath, int isOpenGlFormat /*= 0*/)
{
	if (m_LoadedTexture.find(imageFilePath) == m_LoadedTexture.end()) {
		m_LoadedTexture[imageFilePath] = _CreateTextureFromFile(imageFilePath, isOpenGlFormat);
	}
	Texture2D* tex = m_LoadedTexture[imageFilePath];
	return tex;
}

////////////////////////////////
Texture2D* RenderContext::_CreateTextureFromFile(const char* imageFilePath, int isOpenGlFormat)
{
	Texture2D* textureCreated = new Texture2D(this);
	textureCreated->LoadFromFile(imageFilePath, isOpenGlFormat);
	return textureCreated;
}

////////////////////////////////
TextureView2D* RenderContext::AcquireTextureViewFromFile(const char* imageFilePath, int isOpenGlFormat)
{
	if (m_LoadedTexture.find(imageFilePath) == m_LoadedTexture.end()) {
		m_LoadedTexture[imageFilePath] = _CreateTextureFromFile(imageFilePath, isOpenGlFormat);
	}
	Texture2D* tex = m_LoadedTexture[imageFilePath];
	TextureView2D* view = tex->CreateTextureView();
	return view;
	/*if (m_cachedTextureView.find(tex) == m_cachedTextureView.end()) {
		if (view == nullptr) {
			return nullptr;
		}
		view->SetSampler(m_cachedSamplers[SAMPLER_DEFAULT]);
		m_cachedTextureView[tex] = view;
	}
	return m_cachedTextureView[tex];*/
}

////////////////////////////////
void RenderContext::BindTextureViewWithSampler(unsigned int slot, const TextureView2D* texture) const
{
	BindTextureView(slot, texture);
	if (texture != nullptr) {
		BindSampler(slot, texture->GetSampler());
	} else {
		BindSampler(slot, nullptr);
	}
}

////////////////////////////////
void RenderContext::BindTextureView(unsigned int slot, const TextureView2D* texture) const
{
	ID3D11ShaderResourceView *rsView = nullptr;
	if (texture != nullptr) {
		rsView = texture->GetView();
	} else {
		if (slot == TEXTURE_SLOT_DIFFUSE) {
			Texture2D* white = m_LoadedTexture.find("White")->second;
			rsView = m_cachedTextureView.find(white)->second->GetView();
		} else if (slot == TEXTURE_SLOT_NORMAL) {
			Texture2D* flat = m_LoadedTexture.find("Flat")->second;
			rsView = m_cachedTextureView.find(flat)->second->GetView();
		} else if (slot == TEXTURE_SLOT_EMMISIVE) {
			Texture2D* black = m_LoadedTexture.find("Black")->second;
			rsView = m_cachedTextureView.find(black)->second->GetView();
		} else if (slot == TEXTURE_SLOT_HEIGHT) {
			Texture2D* white = m_LoadedTexture.find("White")->second;
			rsView = m_cachedTextureView.find(white)->second->GetView();
		} else if (slot == TEXTURE_SLOT_SPECULAR) {
			Texture2D* white = m_LoadedTexture.find("White")->second;
			rsView = m_cachedTextureView.find(white)->second->GetView();
		} else  {
			ERROR_AND_DIE("Unable to set texture view from null pointer\n");
		}
	}
	m_context->PSSetShaderResources(slot, 1u, &rsView);
}

////////////////////////////////
void RenderContext::BindSampler(unsigned int slot, Sampler* sampler) const
{
	if (sampler == nullptr) {
		sampler = m_cachedSamplers[SAMPLER_DEFAULT];
	}
	sampler->CreateState(this);
	ID3D11SamplerState *state = sampler->GetSampleState();
	m_context->PSSetSamplers(slot, 1u, &state);

}

////////////////////////////////
void RenderContext::BindSampler(unsigned int slot, PresetSamplers sampler) const
{
	BindSampler(slot, m_cachedSamplers[sampler]);
}

////////////////////////////////
BitmapFont* RenderContext::AcquireBitmapFontFromFile(const char* fontName)
{
	if (m_LoadedFont.find(fontName) == m_LoadedFont.end()) {
		Texture2D* fontTexture = AcquireTextureFromFile((std::string("Data/Fonts/") + fontName + ".png").c_str());
		m_LoadedFont[fontName] = new BitmapFont(fontName, fontTexture);
	}
	return m_LoadedFont[fontName];
}

GPUMesh* RenderContext::AcquireMeshFromFile(const char* meshPath, bool invertWinding/*=false*/)
{
	if(m_LoadedMesh.find(meshPath) == m_LoadedMesh.end()) {
		GPUMesh* mesh = GPUMesh::CreateMeshFromObjFile(this, meshPath, invertWinding);
		m_LoadedMesh[meshPath] = mesh;
		return mesh;
	}
	return m_LoadedMesh[meshPath];
}

////////////////////////////////
void RenderContext::PutMeshIntoCacheUnsafe(const char* meshPath, GPUMesh* mesh)
{
	m_LoadedMesh[meshPath] = mesh;
}

////////////////////////////////
Shader* RenderContext::_CreateShaderFromFile(const char* sourceFilePath, const char* vertEntry, const char* pixelEntry)
{
	Shader* createdShader = new Shader(this);
	// The shader class assume there is only one render context
	//(g_theRenderer) in the world
	createdShader->CreateShaderFromFile(sourceFilePath, vertEntry, pixelEntry);
	if (createdShader != nullptr && createdShader->IsValid()) {
		return createdShader;
	} else {
		delete createdShader;
		ERROR_AND_DIE("Fail to create shader from file\n");
	}
	//return nullptr;
}

////////////////////////////////
Shader* RenderContext::AcquireShaderFromFile(const char* sourceFilePath, const char* vertEntry, const char* pixelEntry)
{
	if (m_LoadedShader.find(sourceFilePath) == m_LoadedShader.end()) {
		m_LoadedShader[sourceFilePath] = _CreateShaderFromFile(sourceFilePath, vertEntry, pixelEntry);
	}
	return m_LoadedShader[sourceFilePath];
}

////////////////////////////////
void RenderContext::SetAmbientLight(const Rgba& color, float intensity)
{
	m_cpuLightBuffer.ambient = color;
	m_cpuLightBuffer.ambient.a = intensity;
	m_lightDirty = true;
}

////////////////////////////////
void RenderContext::SetSpecularFactors(float factor, float power)
{
	m_cpuLightBuffer.specular_factor = factor;
	m_cpuLightBuffer.specular_power = power;
	m_lightDirty = true;
}

////////////////////////////////
void RenderContext::EnableLight(int lightSlot, const Light& lightInfo)
{
	m_cpuLightBuffer.lights[lightSlot] = lightInfo;
	m_lightDirty = true;
}

////////////////////////////////
void RenderContext::DisableLight(int lightSlot)
{
	auto p = &m_cpuLightBuffer.lights[lightSlot];
	memset(p, 0, sizeof(*p));
	m_lightDirty = true;
}

////////////////////////////////
void RenderContext::_loadBlendFunc()
{
	m_currentShader->SetBlendMode(m_blendMode);
}

#endif
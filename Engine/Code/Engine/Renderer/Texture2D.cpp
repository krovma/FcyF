#include "Game/EngineBuildPreferences.hpp"
//////////////////////////////////////////////////////////////////////////
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Core/Job.hpp"
#include <cstring>

////////////////////////////////
unsigned int GetD3DBind(unsigned int usage)
{
	unsigned int bind = 0u
		| ((usage & TEXTURE_USAGE_TEXTURE) ? D3D11_BIND_SHADER_RESOURCE : 0)
		| ((usage & TEXTURE_USAGE_COLOR_TARGET) ? D3D11_BIND_RENDER_TARGET : 0)
		| ((usage & TEXTURE_USAGE_DEPTH_STENCIL) ? D3D11_BIND_DEPTH_STENCIL : 0)
		;
	return bind;
}
////////////////////////////////
static unsigned int __GetUsageFromD3DBind(unsigned int d3dBind)
{
	unsigned int usage = 0u
		| ((d3dBind & D3D11_BIND_SHADER_RESOURCE) ? TEXTURE_USAGE_TEXTURE : 0)
		| ((d3dBind & D3D11_BIND_RENDER_TARGET) ? TEXTURE_USAGE_COLOR_TARGET : 0)
		| ((d3dBind & D3D11_BIND_DEPTH_STENCIL) ? TEXTURE_USAGE_DEPTH_STENCIL : 0)
		;
	return usage;
}
///////////////////////////////
static  GPUMemoryUsage __GetMemoryUsageFromD3DUsage(D3D11_USAGE usage)
{
	switch (usage) {
	case D3D11_USAGE_DEFAULT:
		return GPU_MEMORY_USAGE_GPU;
	case D3D11_USAGE_IMMUTABLE:
		return GPU_MEMORY_USAGE_IMMUTABLE;
	case D3D11_USAGE_DYNAMIC:
		return GPU_MEMORY_USAGE_DYNAMIC;
	case D3D11_USAGE_STAGING:
		return GPU_MEMORY_USAGE_STAGING;
	default:
		return GPU_MEMORY_USAGE_GPU;
	}
}

////////////////////////////////
Texture::Texture(RenderContext* renderer)
	:m_renderer(renderer)
{
}

////////////////////////////////
Texture::~Texture()
{
	//Not a virtual function
	DX_SAFE_RELEASE(m_handle);
}

////////////////////////////////
STATIC Texture2D* Texture2D::CreateDepthStencilTarget(RenderContext* renderer, int width, int height)
{
	Texture2D* createdTexture = new Texture2D(renderer);
	createdTexture->CreateDepthStencilTarget(width, height);
	return createdTexture;
}

////////////////////////////////
STATIC Texture2D* Texture2D::CreateDepthStencilTargetFor(Texture2D* colorTarget)
{
	return CreateDepthStencilTarget(colorTarget->m_renderer, colorTarget->m_textureSize.x, colorTarget->m_textureSize.y);
}

Texture2D* Texture2D::WrapD3DTexture(RenderContext* renderer, ID3D11Texture2D* referenceTexture)
{
	Texture2D* wraped = new Texture2D(renderer);
	D3D11_TEXTURE2D_DESC desc;
	referenceTexture->GetDesc(&desc);
	wraped->m_memoryUsage = __GetMemoryUsageFromD3DUsage(desc.Usage);
	wraped->m_textureUsage = __GetUsageFromD3DBind(desc.BindFlags);
	wraped->m_textureSize = IntVec2(desc.Width, desc.Height);
	wraped->m_handle = referenceTexture;
	referenceTexture->AddRef();
	return wraped;
}

////////////////////////////////
bool Texture2D::CreateDepthStencilTarget(int width, int height)
{
	DX_SAFE_RELEASE(m_handle);
	ID3D11Device* device = m_renderer->GetDevice();
	m_textureUsage = (TEXTURE_USAGE_TEXTURE | TEXTURE_USAGE_DEPTH_STENCIL);
	m_memoryUsage = GPU_MEMORY_USAGE_GPU;
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Usage = GetD3DUsageFromGPUMemoryUsage(m_memoryUsage);
	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.BindFlags = GetD3DBind(m_textureUsage);
	desc.CPUAccessFlags = 0u;
	desc.MiscFlags = 0u;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	ID3D11Texture2D* created = nullptr;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &created);
	if (SUCCEEDED(hr)) {
		m_textureSize.x = width;
		m_textureSize.y = height;
		m_handle = created;
		return true;
	} else {
		ERROR_AND_DIE("Failed to create DepthStencilTarget");
	}
}

////////////////////////////////
DepthStencilTargetView* Texture2D::CreateDepthStencilTargetView() const
{
	if (m_handle == nullptr) {
		ERROR_RECOVERABLE("No texture to create depth target view");
		return nullptr;
	}

	ID3D11Device* device = m_renderer->GetDevice();
	ID3D11DepthStencilView* view = nullptr;

	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	HRESULT hr = device->CreateDepthStencilView(m_handle, &desc, &view);

	if (SUCCEEDED(hr)) {
		DepthStencilTargetView* dst = new DepthStencilTargetView(m_textureSize);
		dst->SetView(view);
		m_handle->AddRef();
		dst->SetHandle(m_handle);
		return dst;

	} else {
		ERROR_RECOVERABLE("Failed to create depth stencil view");
		return nullptr;
	}
}


////////////////////////////////
Texture2D::Texture2D(RenderContext* renderer)
	:Texture(renderer)
{
}

Texture2D::Texture2D(RenderContext* renderer, ID3D11Texture2D* referenceTexture)
	:Texture(renderer)
{
	ID3D11Device* device = renderer->GetDevice();
	m_textureUsage = TEXTURE_USAGE_TEXTURE | TEXTURE_USAGE_COLOR_TARGET;
	m_memoryUsage = GPU_MEMORY_USAGE_GPU;

	D3D11_TEXTURE2D_DESC desc;
	referenceTexture->GetDesc(&desc);
	if (desc.Format == DXGI_FORMAT_R24G8_TYPELESS) {
		m_textureUsage = TEXTURE_USAGE_DEPTH_STENCIL | TEXTURE_USAGE_TEXTURE;
	}
	desc.Usage = GetD3DUsageFromGPUMemoryUsage(GPU_MEMORY_USAGE_GPU);
	desc.BindFlags = GetD3DBind(m_textureUsage);

	ID3D11Texture2D* textureCreated;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &textureCreated);
	if (SUCCEEDED(hr)) {
		m_textureSize = IntVec2(desc.Width, desc.Height);
		m_handle = textureCreated;
	} else {
		ERROR_AND_DIE("Failed to Create texture");
	}
}

////////////////////////////////
Texture2D::~Texture2D()
{
	//Do Nothing
}

class LoadImageJob : public Job
{
public:
	Image* image = nullptr;
	const std::string path;
	int isOpenGlFormat;
	Texture2D* writeTo;
	LoadImageJob(const std::string& path, int isOpenGlFormat, Texture2D* writeTo)
		:path(path), isOpenGlFormat(isOpenGlFormat), writeTo(writeTo)
	{
		SetFinishCallback([](Job* job) {
			LoadImageJob* j = (LoadImageJob*)job;
			if (!j->writeTo->LoadFromImage(j->image)) {
				ERROR_AND_DIE("Cant load from image");
			}
		});
	}

	void Run() override
	{
		image = Image::AcquireImage(path.c_str(), isOpenGlFormat);
	}
};

////////////////////////////////
bool Texture2D::LoadFromFile(const std::string& path, int isOpenGlFormat)
{
	//Image* image = Image::AcquireImage(path.c_str(), isOpenGlFormat);
	LoadImageJob* job = new LoadImageJob(path, isOpenGlFormat, this);
	g_theJobSystem->Run(job);
	return true;
	//return LoadFromImage(image);
}

////////////////////////////////
bool Texture2D::LoadFromImage(Image* image)
{
	DX_SAFE_RELEASE(m_handle);
	ID3D11Device* device = m_renderer->GetDevice();
	m_textureUsage = TEXTURE_USAGE_TEXTURE;
	m_memoryUsage = GPU_MEMORY_USAGE_GPU;
	
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	IntVec2 imageSize = image->GetImageSize();
	desc.Width = (unsigned int)imageSize.x;
	desc.Height = (unsigned int)imageSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Usage = GetD3DUsageFromGPUMemoryUsage(m_memoryUsage);
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BindFlags = GetD3DBind(m_textureUsage);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	unsigned int pitch = (unsigned int)imageSize.x * 4;
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = image->GetRawImageData();
	data.SysMemPitch = pitch;

	ID3D11Texture2D *createdTexture = nullptr;
	HRESULT hr = device->CreateTexture2D(&desc, &data, &createdTexture);
	if (SUCCEEDED(hr)) {
		m_textureSize = imageSize;
		m_handle = createdTexture;
		return true;
	} else {
		ERROR_AND_DIE("Failed when creating texture\n");
		return false;
	}
}

////////////////////////////////
TextureView2D* Texture2D::CreateTextureView() const
{
	//GUARANTEE_OR_DIE((m_handle != nullptr), "Null handle for texture on creating texture view\n");
	if (!this || m_handle == nullptr) {
		return nullptr;
	}
	if (m_view) {
		return m_view;
	}
	ID3D11Device* device = m_renderer->GetDevice();
	ID3D11ShaderResourceView* rsView = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc;
	m_handle->GetDesc(&textureDesc);
	HRESULT hr;
	if (textureDesc.Format == DXGI_FORMAT_R24G8_TYPELESS) {
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(m_handle, &desc, &rsView);
	} else {
		hr = device->CreateShaderResourceView(m_handle, nullptr, &rsView);
	}
	if (SUCCEEDED(hr)) {
		TextureView2D* createdTextureView = new TextureView2D();
		createdTextureView->m_view = rsView;
		m_handle->AddRef();
		createdTextureView->m_resource = m_handle;
		createdTextureView->m_size = m_textureSize;
		
		m_view = createdTextureView;
		return createdTextureView;
	} else {
		ERROR_AND_DIE("Failed to created texture view\n");
	}
}

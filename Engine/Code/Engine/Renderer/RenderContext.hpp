#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include <map>
#include <string>
#include <vector>
#include "Engine/Math/Mat4.hpp"
#define RENDER_DEBUG
//////////////////////////////////////////////////////////////////////////
class Camera;
class RenderTargetView;
class Texture2D;
class TextureView2D;
class Shader;
class RenderBuffer;
class ConstantBuffer;
class VertexBuffer;
class IndexBuffer;
class Sampler;
class GPUMesh;
class DepthStencilTargetView;
class Model;
class Material;
//////////////////////////////////////////////////////////////////////////
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Debug;
struct D3D11_TEXTURE2D_DESC;
//////////////////////////////////////////////////////////////////////////
#include "Engine/Renderer/RenderTypes.hpp"

class RenderContext
{
public:
	struct Light
	{
		Vec3 color=Vec3::ONE; float intensity=0.f;
		Vec3 position=Vec3::ZERO; float isDirectional=0.f;
		Vec3 direction=Vec3::ONE; float _;
		Vec3 diffuseAttenuatio = Vec3::ZERO; float __;
		Vec3 specularAttenuation = Vec3::ZERO; float ___;
	};
	struct LightBufferContent
	{
		Rgba ambient=Rgba::TRANSPARENT_WHITE; //A as intensity
		float specular_factor=0.f; float specular_power=17.32f;  Vec2 _;
		Light lights[8];
	};
public:
	RenderContext(void* hWnd, unsigned int resWidth, unsigned int resHeight);

	// Getters
	ID3D11Device* GetDevice() const { return m_device; }
	ID3D11DeviceContext* GetContext() const { return m_context; }
	IntVec2 GetResolution() const { return m_resolution; }
	// Render stuff
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	RenderTargetView* GetFrameColorTarget() const;
	Texture2D* GetFrameTexture() const { return m_frameTexture; };
	RenderTargetView* GetNewRenderTarget(D3D11_TEXTURE2D_DESC* desc);
	RenderTargetView* GetNewRenderTarget(Texture2D* targetTexture);
	Texture2D* GetNewScratchTextureLike(Texture2D* reference);

	DepthStencilTargetView* GetFrameDepthStencilTarget() const;
	void ClearColorTarget(const Rgba &clearColor) const;
	void ClearDepthStencilTarget(float depth = 1.0f, unsigned char stencil = 0u);
	void CopyTexture(Texture2D* dst, Texture2D* src);
	void ApplyEffect(RenderTargetView* dst, TextureView2D* src, Material* material);


	void BindShader(Shader* shader);
	ConstantBuffer* GetModelBuffer() const { return m_modelBuffer; }
	ConstantBuffer* GetLightBuffer() const { return m_lightBuffer; }
	ConstantBuffer* GetPostBuffer() const { return m_postBuffer; }
	void BindConstantBuffer(ConstantBufferSlot slot, ConstantBuffer* buffer);
	void BindVertexBuffer(VertexBuffer* buffer) const;
	void BindIndexBuffer(IndexBuffer* buffer) const;
	void BeginCamera(Camera &camera);
	void EndCamera(Camera &camera);
	void SetBlendMode(BlendMode mode);

	void Draw(int vertexCount, unsigned int byteOffset = 0u) const;
	void DrawIndexed(int count);
	void DrawVertexArray(int numVertices, const Vertex_PCU vertices[]) const;
	void DrawVertexArray(size_t numVertices, const std::vector<Vertex_PCU>& vertices) const;
	void DrawMesh(const GPUMesh& mesh);
	void DrawModel(const Model& model);

	void Screenshoot(const std::string& path);

	Texture2D* AcquireTextureFromFile(const char* imageFilePath, int isOpenGLFormat = 0);
	TextureView2D* AcquireTextureViewFromFile(const char* imageFilePath, int isOpenGLFormat = 0);
	void BindTextureViewWithSampler(unsigned int slot, const TextureView2D* texture) const;
	void BindTextureView(unsigned int slot, const TextureView2D* texture) const;
	void BindSampler(unsigned int slot, Sampler* sampler) const;
	void BindSampler(unsigned int slot, PresetSamplers sampler) const;
	Shader* AcquireShaderFromFile(const char* sourceFilePath, const char* vertEntry, const char* pixelEntry);
	BitmapFont* AcquireBitmapFontFromFile(const char* fontName);

	GPUMesh* AcquireMeshFromFile(const char* meshPath, bool invertWinding=false);
	void PutMeshIntoCacheUnsafe(const char* meshPath, GPUMesh* mesh);

	//Lighting
	void SetAmbientLight(const Rgba& color, float intensity);
	void SetSpecularFactors(float factor, float power);
	void EnableLight(int lightSlot, const Light& lightInfo);
	void DisableLight(int lightSlot);

private:
	void _loadBlendFunc();
	Texture2D* _CreateTextureFromFile(const char* imageFilePath, int isOpenGLFormat = 0);
	Shader* _CreateShaderFromFile(const char* sourceFilePath, const char* vertEntry, const char* pixelEntry);
	std::map<std::string, Texture2D*> m_LoadedTexture;
	std::map<Texture2D*, TextureView2D*> m_cachedTextureView;
	std::map<std::string, BitmapFont*> m_LoadedFont;
	std::map<std::string, Shader*> m_LoadedShader;
	std::map<std::string, GPUMesh*> m_LoadedMesh;
	BlendMode m_blendMode = BLEND_MODE_ALPHA;
	Camera* m_currentCamera = nullptr;
	Camera* m_effectCamera = nullptr;
	ConstantBuffer* m_modelBuffer = nullptr;
	ConstantBuffer* m_lightBuffer = nullptr;
	ConstantBuffer* m_postBuffer = nullptr;
	LightBufferContent m_cpuLightBuffer;
	bool m_lightDirty=true;
	VertexBuffer* m_immediateVBO = nullptr;
	GPUMesh* m_immediateMesh = nullptr;
	Shader* m_currentShader = nullptr;
	Sampler* m_cachedSamplers[NUM_PRESET_SAMPLERS];
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	RenderTargetView* m_frameRenderTarget = nullptr;
	Texture2D* m_frameTexture = nullptr;
	Texture2D* m_backBuffer = nullptr;
	Texture2D* m_defaultDepthStencilTexture = nullptr;
	DepthStencilTargetView* m_defaultDepthSencilTargetView = nullptr;
	IntVec2 m_resolution;

#if defined(RENDER_DEBUG)
	ID3D11Debug* m_debug = nullptr;
#endif
};

extern RenderContext* g_theRenderer;
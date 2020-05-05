#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/ColorGradient.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

class TextureView2D;
class RenderContext;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
class Camera;
class CPUMesh;
class GPUMesh;
class Shader;

enum DebugRenderMode
{
	DEBUG_RENDER_DEPTH,
	DEBUG_RENDER_ALWAYS,
	DEBUG_RENDER_XRAY,
};

enum DebugRenderRasterizeMode
{
	DEBUG_RENDER_FILL,
	DEBUG_RENDER_WIREFRAME
};

struct DebugRenderObject
{
public:
	DebugRenderObject(RenderContext* renderer);
	~DebugRenderObject();
	CPUMesh* m_cpuMesh = nullptr;
	GPUMesh* m_gpuMesh = nullptr;
	const TextureView2D* m_texture = nullptr;
	DebugRenderMode m_renderMode = DEBUG_RENDER_DEPTH;
	DebugRenderRasterizeMode m_rasterizeMode = DEBUG_RENDER_FILL;
	bool m_isBillboard = false;
	float m_upTime = 0.f;
	float m_lifeTime = -1.f;
	Vec3 m_position;
	ColorGradient m_colorGradient = ColorGradient::WHITE_NOGRADIENT;
	bool m_isTransparent = false;
	bool m_useGradient = true;
};

struct DebugOutputMessage
{
public:
	std::string m_text;
	float m_upTime = 0.f;
	float m_lifeTime = -1.f;
	ColorGradient m_colorGradient = ColorGradient::FADEOUT;
};

class DebugRenderer
{
public:
	// Proper Usages
	static void Startup(RenderContext* renderer);
	static void Shutdown();
	static void BeginFrame();
	static void Update(float deltaSeconds);
	static void Render(Camera* worldCamera);
	static void EndFrame();
	static void Clear();
	static void ToggleRendering(bool isRendering);
	static void ToggleRendering();
	/// Use a negative time to permanently draw a point
	static DebugRenderObject* DrawPoint3D(const Vec3& position, float size, float time=-1.f, const ColorGradient colorGradient=ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawLine3D(const Vec3& start, const Vec3& end, float thickness, float time=-1.f, const ColorGradient colorGradient=ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawArrow3D(const Vec3& start, const Vec3& end, float thickness, float headSize, float time=-1.f, const ColorGradient colorGradient=ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawAABB2In3D(const AABB2& shape, float zBias=0.f, TextureView2D* texture = nullptr, float time = -1.f, const ColorGradient colorGradient = ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawBillboardQuad(const Vec3& center, const AABB2& quadShape, TextureView2D* texture=nullptr, float time=-1.f, const ColorGradient colorGradient=ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawBillboardText(const Vec3& center, const AABB2& textBoxShape, const BitmapFont* font, float cellHeight, const std::string& text, float time=-1.f, const Vec2& alignment=BitmapFont::ALIGNMENT_CENTER, const ColorGradient colorGradient=ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawWireBall(const Vec3& center, float radius, float time=-1.f, const ColorGradient colorGradient=ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawBasis(Mat4 basis, float thickness, float headsize, float time = -1.f);

	static DebugRenderObject* DrawPoint2D(const Vec2& position, float size, float time = -1.f, const ColorGradient colorGradient = ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawLine2D(const Vec2& start, const Vec2& end, float thickness, float time = -1.f, const ColorGradient colorGradient = ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawQuad2D(const AABB2& quad, TextureView2D* texture = nullptr, float time = -1.f, const ColorGradient colorGradient = ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawText2D(const AABB2& textBox, const BitmapFont* font, float cellHeight, const std::string& text, float time = -1.f, const Vec2& alignment = BitmapFont::ALIGNMENT_CENTER, const ColorGradient colorGradient = ColorGradient::WHITE_NOGRADIENT);
	static DebugRenderObject* DrawCameraBasisOnScreen(const Camera& camera, float time = -1.f);
	static void Log(const std::string& text, float time = 2.f, const ColorGradient colorGradient=ColorGradient::FADEOUT);
	static void ClearLog();

private:
	static DebugRenderer* s;

private:
	DebugRenderer(RenderContext* renderer);
	~DebugRenderer();
	ID3D11DepthStencilState* m_dsAlways = nullptr;
	ID3D11DepthStencilState* m_dsUseDepth = nullptr;
	ID3D11DepthStencilState* m_dsTransparent = nullptr;
	ID3D11DepthStencilState* m_dsXray = nullptr;
	ID3D11RasterizerState* m_rsNormal = nullptr;
	ID3D11RasterizerState* m_rsWireframe = nullptr;

	inline ID3D11DepthStencilState* GetDepthStencilState(DebugRenderMode mode)
	{
		switch (mode) {
		case DEBUG_RENDER_ALWAYS: return m_dsAlways;
		case DEBUG_RENDER_DEPTH:  return m_dsUseDepth;
		case DEBUG_RENDER_XRAY:   return m_dsXray;
		default: return m_dsAlways;
		}
	}
	inline ID3D11RasterizerState* GetRasterizerState(DebugRenderRasterizeMode mode)
	{
		switch (mode) {
		case DEBUG_RENDER_FILL:		 return m_rsNormal;
		case DEBUG_RENDER_WIREFRAME: return m_rsWireframe;
		default: return m_rsNormal;
		}
	}


private:
	RenderContext* m_renderer;
	Shader* m_shader;
	bool m_isRendering=true;
	Camera* m_screenCamera=nullptr;
	std::vector<DebugRenderObject*> m_WorldObjects;
	std::vector<DebugRenderObject*> m_screenObjects;
	std::vector<DebugOutputMessage> m_messages;
};
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/RenderTargetView.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Math/Ray.hpp"
//////////////////////////////////////////////////////////////////////////
class Camera
{
public:
	static Mat4 MakePerspectiveProjection(float fovDegrees, float aspect, float nz, float fz);
	static Mat4 MakeOrthogonalProjection(const Vec2& orthoMin, const Vec2& orthoMax, float nearZ, float farZ);
	static Mat4 MakeLookAtModel(const Vec3& from, const Vec3& to, const Vec3& up);
public:
	struct __CameraConstantBuffer
	{
		Mat4 projection;
		//Mat4 camera;
		Mat4 view;
		Mat4 ModelProj;
		Vec3 position;
		float _;
	};

	Camera();
	~Camera();
	explicit Camera(const Vec2 &orthoMin, const Vec2 &orthoMax);
public:
	void SetOrthoView(const Vec2 &orthoMin, const Vec2 &orthoMax, float nearZ=1.f, float farZ=-1.f);
	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	
	void Translate2D(const Vec2 &translate);
	
	void SetRenderTarget(RenderTargetView* renderTarget) { m_renderTarget = renderTarget; }
	void SetDepthStencilTarget(DepthStencilTargetView* dsv);
	RenderTargetView* GetRenderTarget() const { return m_renderTarget; }
	DepthStencilTargetView* GetDepthStencilTargetView() const { return m_depthStencilTarget; }
	void UpdateConstantBuffer(RenderContext* renderer);
	ConstantBuffer* GetConstantBuffer() const { return m_cameraUBO; }

	Vec2 ClientToWorld(const Vec2& screen) const;
	Ray3 ClientToWorldRay3(const IntVec2& screen) const;
	Vec2 WorldToClient(const Vec2& world) const;

	Mat4 GetCameraModel() const { return m_model; }
	void SetCameraModel(const Mat4& model);
	void SetProjection(const Mat4& projection);
	void SetResolution(const Vec2& resolution) { m_resolution = resolution; }
private:
	Vec2 m_orthoMin;
	Vec2 m_orthoMax;
	Vec2 m_resolution;
	Mat4 m_projection = Mat4::Identity;
	Mat4 m_camera = Mat4::Identity;
	Mat4 m_view = Mat4::Identity;
	Mat4 m_model = Mat4::Identity;
	
	RenderTargetView* m_renderTarget = nullptr;
	DepthStencilTargetView* m_depthStencilTarget = nullptr;
	ConstantBuffer* m_cameraUBO = nullptr;
};

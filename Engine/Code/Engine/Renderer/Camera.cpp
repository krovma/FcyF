#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Develop/DebugRenderer.hpp"

////////////////////////////////
Mat4 Camera::MakePerspectiveProjection(float fovDegrees, float aspect, float nearZ, float farZ)
{
	float fovSize = 1.f / TanDegrees(fovDegrees / 2.f);
	float width = fovSize / aspect;
	float height = fovSize;
	float ratio = 1.f / (farZ - nearZ);
	Vec4 i(width, 0.f, 0.f, 0.f);
	Vec4 j(0.f, height, 0.f, 0.f);
	// Reverse K (and near/far) form right hand system to left hand system (d3d)
	// https://docs.microsoft.com/en-us/windows/desktop/direct3d9/coordinate-systems
	Vec4 k(0.f, 0.f, -ratio * nearZ, -1.f);
	Vec4 t(0.f, 0.f, -nearZ * farZ * ratio, 0.f);
	return Mat4(i, j, k, t);
}

////////////////////////////////
Mat4 Camera::MakeOrthogonalProjection(const Vec2& orthoMin, const Vec2& orthoMax, float nearZ, float farZ)
{
	float xRatio = 1.f / (orthoMax.x - orthoMin.x);
	float yRatio = 1.f / (orthoMax.y - orthoMin.y);
	float zRatio = 1.f / (farZ - nearZ);
	Vec4 i(2.f * xRatio, 0.f, 0.f, -(orthoMin.x + orthoMax.x) * xRatio);
	Vec4 j(0.f, 2.f * yRatio, 0.f, -(orthoMin.y + orthoMax.y) * yRatio);
	Vec4 k(0.f, 0.f, -zRatio, farZ * zRatio);
	Vec4 t(0.f, 0.f, 0.f, 1.f);
	return Mat4(i, j, k, t).GetTransposed();
}

Mat4 Camera::MakeLookAtModel(const Vec3& from, const Vec3& to, const Vec3& up)
{
	Vec3 k = from - to;
	k.Normalize();
	Vec3 i = up.CrossProduct(k);
	i.Normalize();

	return Mat4(i, up, k, from);
}

Camera::Camera()
{
}

Camera::Camera(const Vec2 &orthoMin, const Vec2 &orthomax)
	: m_orthoMin(orthoMin)
	, m_orthoMax(orthomax)
{
	SetOrthoView(m_orthoMin, m_orthoMax, 100.f, -100.f);
}

////////////////////////////////
Camera::~Camera()
{
	delete m_cameraUBO;
}

//////////////////////////////////////////////////////////////////////////
void Camera::SetOrthoView(const Vec2 &orthoMin, const Vec2 &orthoMax, float nearZ, float farZ)
{
	m_projection = MakeOrthogonalProjection(orthoMin, orthoMax, nearZ, farZ);
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetOrthoBottomLeft() const
{
	return Vec2(m_orthoMin);
}

//////////////////////////////////////////////////////////////////////////
Vec2 Camera::GetOrthoTopRight() const
{
	return Vec2(m_orthoMax);
}

////////////////////////////////
void Camera::Translate2D(const Vec2 &translate)
{
	m_orthoMin += translate;
	m_orthoMax += translate;
	SetOrthoView(m_orthoMin, m_orthoMax);
}

////////////////////////////////
void Camera::SetDepthStencilTarget(DepthStencilTargetView* dsv)
{
	m_depthStencilTarget = dsv;
}

////////////////////////////////
void Camera::UpdateConstantBuffer(RenderContext* renderer)
{
	if (m_cameraUBO == nullptr) {
		m_cameraUBO = new ConstantBuffer(renderer);
	}
	__CameraConstantBuffer cameraData;
	cameraData.projection = m_projection;
	cameraData.view = m_view;
	cameraData.ModelProj = m_view.GetInverted() * m_projection.GetInverted();
	cameraData.position = m_model.GetT().XYZ() / m_model.GetT().w;
	m_cameraUBO->Buffer(&cameraData, sizeof(cameraData));
}

////////////////////////////////
Vec2 Camera::ClientToWorld(const Vec2& screen) const
{
	Vec4 pos(screen, 0.f, 1.f);
	pos.x = FloatMap(pos.x, 0.f, m_resolution.x, -1.f, 1.f);
	pos.y = FloatMap(pos.y, 0.f, m_resolution.y, 1.f, -1.f);
	Vec4 homo = (m_projection * m_view).GetInverted()  * pos;
	homo /= homo.w;
	return Vec2(homo.x, homo.y);
}

Ray3 Camera::ClientToWorldRay3(const IntVec2& screen) const
{
	Vec4 pos((float)screen.x, (float)screen.y, -1.f, 1.f);
	pos.x = FloatMap(pos.x, 0.f, m_resolution.x, -1.f, 1.f);
	pos.y = FloatMap(pos.y, 0.f, m_resolution.y, -1.f, 1.f); 
	Vec4 nearHomo = (m_projection * m_view).GetInverted()  * pos;

	pos.z = 0.f;
	Vec4 farHomo = (m_projection * m_view).GetInverted()  * pos;

	nearHomo /= nearHomo.w;
	farHomo /= farHomo.w;
	//farHomo.z *= -1.f;
	//DebugRenderer::DrawLine3D(nearHomo.XYZ(), -(farHomo - nearHomo).XYZ()*100.f + nearHomo.XYZ(), 0.02f, 5.f, ColorGradient(Rgba::WHITE, Rgba::RED));
	return Ray3(nearHomo.XYZ(), -(farHomo - nearHomo).XYZ());
}

////////////////////////////////
Vec2 Camera::WorldToClient(const Vec2& world) const
{
	Vec4 homo(world, 0.f, 1.f);
	Vec4 ndc = m_projection * m_view * homo;
	ndc /= ndc.w;
	Vec2 client = Vec2(
		FloatMap(ndc.x, -1.f, 1.f, 0.f, 1.f) * m_resolution.x,
		FloatMap(ndc.y, -1.f, 1.f, 1.f, 0.f) * m_resolution.y
	);
	return client;
}

////////////////////////////////
void Camera::SetCameraModel(const Mat4& model)
{
	m_view = model.GetInverted();
	m_model = model;

}

////////////////////////////////
void Camera::SetProjection(const Mat4& projection)
{
	m_projection = projection;
}

#endif // 
#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"
#include <vector>
struct VertexMaster
{
	Vec3 Position=Vec3::ZERO;
	Rgba Color=Rgba::WHITE;
	Vec2 UV=Vec2::ZERO;
	Vec3 Normal = Vec3(0,0,1);
	Vec3 Tangent = Vec3(1, 0, 0);
	VertexMaster() = default;
	VertexMaster(const Vec3 &position, const Rgba& color, const Vec2& uv)
		: Position(position), Color(color), UV(uv), Normal(0,0,1)
	{}
};

class CPUMesh
{
public:
	static CPUMesh* CreateCPUMeshFromObjFile(const char* path, bool invertWinding = false);
public:
	CPUMesh(const RenderBufferLayout* layout);
	CPUMesh(const CPUMesh& copyFrom);
	void Clear();
	void ClearIndices();

	void ResetBrush();
	void SetBrushColor(const Rgba& color) { m_brush.Color = color; }
	void SetBrushUV(const Vec2& uv) { m_brush.UV = uv; };
	void SetBrushNormal(const Vec3& normal) { m_brush.Normal = normal; }
	void SetBrushTangent(const Vec3& tangent) { m_brush.Tangent = tangent; }
	int AddVertex(const VertexMaster& vertex);
	int AddVertex(const Vec3& position);
	void AddVertexAndIndex(const Vec3& position);
	void SetVertexUVByIndex(int index, const Vec2& uv);
	void SetVertexColorByIndex(int index, const Rgba& color);

	//void AddTriangle3D(const Vec3& vert0, const Vec3& vert1, const Vec3& vert2);
	void AddLine2DToMesh(const Vec2& start, const Vec2& end, float thickness);
	void AddAABB2ToMesh(const AABB2& quad);
	void AddDiskToMesh(const Vec2& center, float radius, int slice=32);
	void AddCubeToMesh(const AABB3& box);
	void AddUVSphereToMesh(const Vec3& center, float radius, int longitude = 32, int latitude = 16);
	void AddTriangleByIndices(int vert0, int vert1, int vert2);
	void AddTriangle(const VertexMaster& a, const VertexMaster& b, const VertexMaster& c);
	void AddQuad3D(const Vec3& topLeft, const Vec3& topRight, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& normal=Vec3(0,0,1), const Vec3& tangent=Vec3(1,0,0));
	void AddQuad3D(const VertexMaster& topLeft, const VertexMaster& bottomLeft, const VertexMaster& bottomRight, const VertexMaster& topRight);
	void AddQuadByIndices(int topLeft, int topRight, int bottomLeft, int bottomRight);
	void AddCylinderToMesh(const Vec3& start, const Vec3& end, float radius, int longitude = 16, int latitude = 3);
	void AddCapsuleToMesh(const Vec3& start, const Vec3& end, float radius, int longitude = 16, int latitude = 6);
	void AddConeToMesh(const Vec3& center, float radius, const Vec3& apex, int slice = 16);
	void AddZPlane3D(const Vec3& bottomLeft, const Vec3& topRight, int xStep = 1, int yStep = 1);

	int GetVertexCount() const { return (int)m_vertices.size(); }
	int GetIndicesCount() const { return (int)m_indices.size();  }
	VertexMaster GetVertexByIndex(int index) const { return m_vertices[index]; }
	VertexMaster& GetVertexByIndex(int index) { return m_vertices[index]; }
	int GetIndexByIndex(int index) const { return m_indices[index]; }
	const int* GetIndicesDataBuffer() const { return m_indices.data(); }
	const VertexMaster* GetVertexBuffer() const { return m_vertices.data(); }
	std::vector<VertexMaster>& GetRawData() { return m_vertices; }

	bool IsUsingIndexBuffer() const { return GetIndicesCount() > 0; }
	int GetElementCount() const { return IsUsingIndexBuffer() ? GetIndicesCount() : GetVertexCount(); }
	
	void SetLayout(const RenderBufferLayout* layout) { m_layout = layout; }
	const RenderBufferLayout* GetLayout() const { return m_layout; }


private:
	std::vector<VertexMaster> m_vertices;
	std::vector<int> m_indices;
	VertexMaster m_brush;
	const RenderBufferLayout* m_layout=nullptr;
};
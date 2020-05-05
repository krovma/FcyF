#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Core/Vertex_PCUNT.hpp"
#include <iostream>
#include <fstream>
////////////////////////////////

//////////////////////////////// 
STATIC CPUMesh* CPUMesh::CreateCPUMeshFromObjFile(const char* path, bool invertWinding /*= false*/)
{
	CPUMesh* mesh = new CPUMesh(RenderBufferLayout::AcquireLayoutFor<Vertex_PCUNT>());
	std::vector<Vec3> position;
	std::vector<Vec3> normal;
	std::vector<Vec2> uv;
	std::ifstream fin(path);
	position.emplace_back(Vec3::ZERO);
	normal.emplace_back(Vec3::ZERO);
	uv.emplace_back(Vec2::ZERO);	//this is also the default uv for without texture coordinate

	VertexMaster brush;
	std::vector<VertexMaster> face;
	face.resize(4);
	for (std::string line; std::getline(fin, line);) {
		if (line[0] == '#' || line.empty()) {
			//Comments
			continue;
		}
		const auto splited = Split(line.c_str());
		if (splited[0] == "v") {
			//vertex
			const float x = (float)atof(splited[1].c_str());
			const float y = (float)atof(splited[2].c_str());
			const float z = (float)atof(splited[3].c_str());
			position.emplace_back(Vec3(x, y, z));
		} else if (splited[0] == "vt") {
			//texture uv
			const float u = (float)atof(splited[1].c_str());
			const float v = 1.f - (float)atof(splited[2].c_str());
			uv.emplace_back(Vec2(u, v));
		} else if (splited[0] == "vn") {
			//normal
			const float x = (float)atof(splited[1].c_str());
			const float y = (float)atof(splited[2].c_str());
			const float z = (float)atof(splited[3].c_str());
			normal.emplace_back(Vec3(x, y, z).GetNormalized());
		} else if (splited[0] == "f") {
			if (splited.size() == 5) {
				//Quad
				for (int i = 1; i < 5; ++i) {
					const auto vertex = Split(splited[i].c_str(), '/', false, false);
					brush.Position = position[atoi(vertex[0].c_str())];
					brush.UV = uv[atoi(vertex[1].c_str())];
					brush.Normal = normal[atoi(vertex[2].c_str())];
					face[i - 1] = brush;
				}
				if (invertWinding) {
					mesh->AddTriangle(face[0], face[2], face[1]);
					mesh->AddTriangle(face[0], face[3], face[2]);
				} else {
					mesh->AddQuad3D(face[0], face[1], face[2], face[3]);
				}

			} else if (splited.size() == 4) {
				//Triangle
				for (int i = 1; i < 4; ++i) {
					const auto vertex = Split(splited[i].c_str(), '/', false, false);
					brush.Position = position[atoi(vertex[0].c_str())];
					brush.UV = uv[atoi(vertex[1].c_str())];
					brush.Normal = normal[atoi(vertex[2].c_str())];
					face[i - 1] = brush;
				}
				if (invertWinding) {
					mesh->AddTriangle(face[0], face[2], face[1]);
				} else {
					mesh->AddTriangle(face[0], face[1], face[2]);
				}
			}
		}
	}
	return mesh;
}
////////////////////////////////
void CPUMesh::AddAABB2ToMesh(const AABB2& quad)
{
	SetBrushUV(Vec2(0.f, 0.f));
	int tl = AddVertex(Vec3(quad.GetTopLeft()));
	SetBrushUV(Vec2(1.f, 0.f));
	int tr = AddVertex(Vec3(quad.GetTopRight()));
	SetBrushUV(Vec2(0.f, 1.f));
	int bl = AddVertex(Vec3(quad.GetBottomLeft()));
	SetBrushUV(Vec2(1.f, 1.f));
	int br = AddVertex(Vec3(quad.GetBottomRight()));
	AddQuadByIndices(tl, tr, bl, br);
}

////////////////////////////////
void CPUMesh::AddDiskToMesh(const Vec2& center, float radius, int slice)
{
	int* index = new int[slice + 2];

	SetBrushUV(Vec2(0.5f, 0.5f));
	index[0] = AddVertex(Vec3(center));
	for (int i = 0; i < slice; ++i) {
		float theta = (float)i / (float)slice * 360.f;
		Vec2 UV;
		UV.x = CosDegrees(theta);
		UV.y = SinDegrees(theta);
		Vec2 position = radius * UV + center;
		UV.y = 1 - UV.y;
		UV = UV * 0.5f;
		UV.x += 0.5f;
		UV.y += 0.5f;
		SetBrushUV(UV);
		index[i + 1] = AddVertex(Vec3(position));
	}
	index[slice + 1] = index[1];
	for (int i = 1; i <= slice; ++i) {
		AddTriangleByIndices(index[0], index[i], index[i + 1]);
	}

	delete[] index;
}

////////////////////////////////
void CPUMesh::AddLine2DToMesh(const Vec2& start, const Vec2& end, float thickness)
{
	SetBrushUV(Vec2(0, 0));
	Vec2 right = (end-start).GetNormalized().GetRotatedMinus90Degrees();
	right.SetLength(thickness * 0.5f);

	Vec2 tr = end + right;
	Vec2 tl = end - right;
	Vec2 br = start + right;
	Vec2 bl = start - right;

	AddQuad3D(Vec3(tl), Vec3(tr), Vec3(bl), Vec3(br));
}

////////////////////////////////
void CPUMesh::AddCubeToMesh(const AABB3& box)
{
	Vec3 position[8];
	//int tL, tR, bL, bR;
	//  y3   2
	//  |0__x1
	//7/  6
	//4z  5
	// Winding order = CCW
	position[0].x = box.Min.x; position[0].y = box.Min.y; position[0].z = box.Min.z;
	position[1].x = box.Max.x; position[1].y = box.Min.y; position[1].z = box.Min.z;
	position[2].x = box.Max.x; position[2].y = box.Max.y; position[2].z = box.Min.z;
	position[3].x = box.Min.x; position[3].y = box.Max.y; position[3].z = box.Min.z;
	position[4].x = box.Min.x; position[4].y = box.Min.y; position[4].z = box.Max.z;
	position[5].x = box.Max.x; position[5].y = box.Min.y; position[5].z = box.Max.z;
	position[6].x = box.Max.x; position[6].y = box.Max.y; position[6].z = box.Max.z;
	position[7].x = box.Min.x; position[7].y = box.Max.y; position[7].z = box.Max.z;

	SetBrushColor(Rgba::WHITE);
	AddQuad3D(position[2], position[3], position[1], position[0], Vec3(0, 0, -1), Vec3(-1, 0, 0));// back
	//SetBrushColor(Rgba::GREEN);
	AddQuad3D(position[3], position[7], position[0], position[4], Vec3(-1, 0, 0), Vec3(0, 0, 1));// left
	//SetBrushColor(Rgba::BLUE);
	AddQuad3D(position[6], position[2], position[5], position[1], Vec3(1, 0, 0), Vec3(0, 0, -1));// right
	//SetBrushColor(Rgba::CYAN);
	AddQuad3D(position[3], position[2], position[7], position[6], Vec3(0, 1, 0), Vec3(1, 0, 0));// top
	//SetBrushColor(Rgba::MAGENTA);
	AddQuad3D(position[1], position[0], position[5], position[4], Vec3(0, -1, 0), Vec3(-1, 0, 0));// down
	//SetBrushColor(Rgba::YELLOW);
	AddQuad3D(position[7], position[6], position[4], position[5], Vec3(0, 0, 1), Vec3(1, 0, 0));// front
}

////////////////////////////////
void CPUMesh::AddUVSphereToMesh(const Vec3& center, float radius, int longitude /*= 32*/, int latitude /*= 16*/)
{
	int uStep = longitude + 1;
	int vStep = latitude + 1;
	int* indices = new int[uStep * vStep];
	ResetBrush();
	for (int v = 0; v < vStep; ++v) {
		for (int u = 0; u < uStep; ++u) {
			Vec2 UV;
			UV.x = (float)u / (float)longitude;
			UV.y = (float)v / (float)latitude;
			float theta = -360.f * UV.x;
			float phi = FloatMap(180.f * UV.y, 0.f, 180.f, 90.f, -90.f);
			Vec3 position(
				radius * CosDegrees(phi) * CosDegrees(theta),
				radius * SinDegrees(phi),
				radius * CosDegrees(phi) * SinDegrees(theta)
			);
			SetBrushUV(UV);
			SetBrushNormal(position.GetNormalized());
			indices[v * uStep + u] = AddVertex(position + center);
		}
	}

	for (int v = 0; v < latitude; ++v) {
		for (int u = 0; u < longitude; ++u) {
			int tl = v * uStep + u;

			const Vec3& posTL = m_vertices[indices[tl]].Position - center;
			const Vec3& posBL = m_vertices[indices[tl+uStep]].Position - center;
			Vec3 tangent = posTL.CrossProduct(posBL).GetNormalized();
			m_vertices[indices[tl]].Tangent = tangent;
			m_vertices[indices[tl+uStep]].Tangent = tangent;
			AddQuadByIndices(
				indices[tl],
				indices[tl + 1],
				indices[tl + uStep],
				indices[tl + uStep + 1]
			);
		}
	}

	for (int v = 0; v < latitude; ++v) {
		m_vertices[indices[(v + 1) * uStep - 1]].Tangent = m_vertices[indices[v * uStep]].Tangent;
	}

	delete[] indices;
}

////////////////////////////////
CPUMesh::CPUMesh(const CPUMesh& copyFrom)
	: m_vertices(copyFrom.m_vertices)
	, m_indices(copyFrom.m_indices)
	, m_brush(copyFrom.m_brush)
	, m_layout(copyFrom.m_layout)
{
}

////////////////////////////////
CPUMesh::CPUMesh(const RenderBufferLayout* layout)
	:m_layout(layout)
{
}

////////////////////////////////
void CPUMesh::Clear()
{
	m_vertices.clear();
	m_indices.clear();
	m_brush = VertexMaster();
}

////////////////////////////////
void CPUMesh::ClearIndices()
{
	m_indices.clear();
}

////////////////////////////////
void CPUMesh::ResetBrush()
{
	m_brush = VertexMaster();
}

////////////////////////////////
int CPUMesh::AddVertex(const Vec3& position)
{
	m_brush.Position = position;
	return AddVertex(m_brush);
}

////////////////////////////////
int CPUMesh::AddVertex(const VertexMaster& vertex)
{
	m_vertices.push_back(vertex);
	return (int)m_vertices.size() - 1;
}

////////////////////////////////
void CPUMesh::AddVertexAndIndex(const Vec3& position)
{
	m_indices.push_back(AddVertex(position));
}

////////////////////////////////
void CPUMesh::AddTriangleByIndices(int vert0, int vert1, int vert2)
{
	int size = (int)m_vertices.size();
	GUARANTEE_OR_DIE(
		(size > vert0) && (size > vert1) && (size > vert2),
		"Vertex index too large! [CPUMesh::AddIndexedTriangle]"
	);
	m_indices.push_back(vert0);
	m_indices.push_back(vert1);
	m_indices.push_back(vert2);
}

void CPUMesh::AddTriangle(const VertexMaster& a, const VertexMaster& b, const VertexMaster& c)
{
	m_vertices.push_back(a);
	m_vertices.push_back(b);
	m_vertices.push_back(c);

}

void CPUMesh::AddQuad3D(
	const VertexMaster& topLeft, const VertexMaster& bottomLeft, const VertexMaster& bottomRight
	, const VertexMaster& topRight)
{
	AddTriangle(topLeft, bottomLeft, bottomRight);
	AddTriangle(topLeft, bottomRight, topRight);
}

////////////////////////////////
void CPUMesh::AddQuad3D(const Vec3& topLeft, const Vec3& topRight, const Vec3& bottomLeft, const Vec3& bottomRight, const Vec3& normal, const Vec3& tangent)
{
	SetBrushNormal(normal);
	SetBrushTangent(tangent);
	SetBrushUV(Vec2(0.f, 0.f));
	int tl = AddVertex(topLeft);
	SetBrushUV(Vec2(1.f, 0.f));
	int tr = AddVertex(topRight);
	SetBrushUV(Vec2(0.f, 1.f));
	int bl = AddVertex(bottomLeft);
	SetBrushUV(Vec2(1.f, 1.f));
	int br = AddVertex(bottomRight);
	AddQuadByIndices(tl, tr, bl, br);
}

////////////////////////////////
void CPUMesh::AddQuadByIndices(int topLeft, int topRight, int bottomLeft, int bottomRight)
{
	AddTriangleByIndices(bottomLeft, topRight, topLeft);
	AddTriangleByIndices(bottomLeft, bottomRight, topRight);
}

////////////////////////////////
void CPUMesh::AddCylinderToMesh(const Vec3& start, const Vec3& end, float radius, int longitude /*= 16*/, int latitude /*= 3*/)
{
	int uStep = longitude + 1;
	int vStep = latitude;
	float height = (end - start).GetLength();
	// gen a cylinder with 1 radius and along z axis, then transform to correct position;
	Vec3 direction = end - start;
	Mat4 rotationXYZ = GetRotationXYZFromAToB(Vec3(0, 0, 1), direction);
	Mat4 model = Mat4::MakeTranslate3D(start) * rotationXYZ * Mat4::MakeScale3D(radius, radius, height);

	int* indices = new int[uStep * vStep + 2];
	SetBrushUV(Vec2::ZERO);
	SetBrushNormal((model * Vec4(0, 0, -1, 0)).XYZ());
	SetBrushTangent((model * Vec4(1, 0, 0, 0)).XYZ());
	indices[0] = AddVertex((model * Vec4(Vec3::ZERO, 1.f)).XYZ());
	for (int v = 0; v < vStep; ++v) {
		for (int u = 0; u < uStep; ++u) {
			Vec2 UV;
			UV.x = (float)u / (float)longitude;
			UV.y = (float)v / (float)(latitude - 1);
			float theta = UV.x * 360.f;
			float z = UV.y;
			SetBrushUV(UV);
			Vec3 localPosition = Vec3(CosDegrees(theta), SinDegrees(theta), z);
			Vec3 localNormal = Vec3(CosDegrees(theta), SinDegrees(theta), 0);
			Vec3 localTangent = Vec3(-SinDegrees(theta), CosDegrees(theta), 0);
			SetBrushNormal((model * Vec4(localNormal, 0.f)).XYZ());
			SetBrushTangent((model * Vec4(localTangent, 0.f)).XYZ());
			indices[v * uStep + u + 1/*the start point*/] = AddVertex((model * Vec4(localPosition, 1.f)).XYZ());
		}
	}
	SetBrushUV(Vec2::ONE);
	SetBrushNormal((model * Vec4(0, 0, 1, 0)).XYZ());
	SetBrushTangent((model * Vec4(1, 0, 0, 0)).XYZ());
	indices[uStep * vStep + 1] = AddVertex((model * Vec4(0, 0, 1.f, 1.f)).XYZ());

	for (int v = 0; v < latitude - 1; ++v) {
		for (int u = 0; u < longitude; ++u) {
			int tr = v * uStep + u + 1;
			AddQuadByIndices(
				indices[tr + 1],
				indices[tr],
				indices[tr + uStep + 1],
				indices[tr + uStep]
			);
		}
	}
	for (int u = 0; u < longitude; ++u) {
		AddTriangleByIndices(indices[0], indices[(u + 1) + 1], indices[(u + 1)]);
		AddTriangleByIndices(indices[uStep * vStep + 1], indices[(vStep - 1) * uStep + u + 1], indices[((vStep - 1) * uStep + u + 1) + 1]);
	}

	delete[] indices;
}

void CPUMesh::AddCapsuleToMesh(const Vec3& start, const Vec3& end, float radius, int longitude, int latitude)
{
	AddCylinderToMesh(start, end, radius, longitude, latitude);
	AddUVSphereToMesh(start, radius, longitude, latitude);
	AddUVSphereToMesh(end, radius, longitude, latitude);
}

////////////////////////////////
void CPUMesh::AddConeToMesh(const Vec3& center, float radius, const Vec3& apex, int slice /*= 16*/)
{
	//Calculate the model from orientation 0 0 1 to apex-center
	Mat4 rotation = GetRotationXYZFromAToB(Vec3(0, 0, 1), apex - center);
	Mat4 model = Mat4::MakeTranslate3D(center) * rotation * Mat4::MakeScale3D(radius, radius, (apex - center).GetLength());

	int uStep = slice + 1;
	int* indices = new int[uStep + 2];
	SetBrushUV(Vec2(0.5f, 1.f));
	indices[0] = AddVertex((model * Vec4(0,0,0,1.f)).XYZ());
	for (int u = 0; u < uStep; ++u) {
		Vec2 UV;
		UV.x = (float)u / (float)slice;
		UV.y = 1.f;
		float theta = UV.x * 360.f;
		SetBrushUV(UV);
		Vec3 localPosition(CosDegrees(theta), SinDegrees(theta), 0.f);
		indices[u + 1] = AddVertex((model * Vec4(localPosition, 1.f)).XYZ());
	}
	SetBrushUV(Vec2(0.5f, 0.f));
	indices[uStep + 1] = AddVertex((model * Vec4(0, 0, 1, 1.f)).XYZ());

	for (int u = 0; u < slice; ++u) {
		AddTriangleByIndices(indices[0], indices[u + 1 + 1], indices[u + 1]);
		AddTriangleByIndices(indices[uStep + 1], indices[u + 1], indices[u + 1 + 1]);
	}
	delete[] indices;
}

void CPUMesh::AddZPlane3D(const Vec3& bottomLeft, const Vec3& topRight, int xStep, int yStep)
{
	int* indices = new int[(xStep + 1) * (yStep + 1)];
	float z = (bottomLeft.z + topRight.z) * 0.5f;
	Vec3 unit = topRight - bottomLeft;
	unit.x /= (float)xStep;
	unit.y /= (float)yStep;
	SetBrushColor(Rgba::WHITE);
	SetBrushTangent(Vec3(0, 1, 0));
	SetBrushNormal(Vec3(0, 0, 1));
	for (int y = 0; y <= yStep; ++y) {
		for (int x = 0; x <= xStep; ++x) {
			Vec2 UV((float)x, (float)y);
			//UV.y = yStep - UV.y;
			SetBrushUV(UV);
			Vec3 position;
			position.x = bottomLeft.x + (float)x * unit.x;
			position.y = bottomLeft.y + (float)y * unit.y;
			position.z = z;
			indices[y * (xStep + 1) + x] = AddVertex(position);
		}
	}
	for (int y = 0; y < yStep; ++y) {
		for (int x = 0; x < xStep; ++x) {
			AddQuadByIndices(
				indices[(y + 1) * (xStep + 1) + x]
				, indices[(y + 1) * (xStep + 1) + x + 1]
				, indices[(y) * (xStep + 1) + x]
				, indices[(y) * (xStep + 1) + x + 1]
			);
		}
	}
	delete[] indices;
}

////////////////////////////////
void CPUMesh::SetVertexUVByIndex(int index, const Vec2& uv)
{
	if (index >= m_vertices.size()) {
		ERROR_AND_DIE("index too larget [CPUMesh::SetVertexUV]");
	}
	m_vertices[index].UV = uv;
}

////////////////////////////////
void CPUMesh::SetVertexColorByIndex(int index, const Rgba& color)
{
	if (index >= m_vertices.size()) {
		ERROR_AND_DIE("index too larget [CPUMesh::SetVertexUV]");
	}
	m_vertices[index].Color = color;
}


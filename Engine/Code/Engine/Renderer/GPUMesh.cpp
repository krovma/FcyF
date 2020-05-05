#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUNT.hpp"
#include <vector>
#include <fstream>

////////////////////////////////////////
STATIC GPUMesh* GPUMesh::CreateMeshFromObjFile(RenderContext* renderer, const char* path, bool invertWinding/*=false*/)
{
	CPUMesh mesh(RenderBufferLayout::AcquireLayoutFor<Vertex_PCUNT>());
	GPUMesh* loaded = nullptr;

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
		if(line[0] == '#' || line.empty()) {
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
					mesh.AddTriangle(face[0], face[2], face[1]);
					mesh.AddTriangle(face[0], face[3], face[2]);
				} else {
					mesh.AddQuad3D(face[0], face[1], face[2], face[3]);
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
					mesh.AddTriangle(face[0], face[2], face[1]);
				} else {
					mesh.AddTriangle(face[0], face[1], face[2]);
				}
			}
		}
	}
	//Split()
	loaded = new GPUMesh(renderer);
	loaded->SetLayout(mesh.GetLayout());
	loaded->CreateFromCPUMesh(mesh);
	return loaded;
}

////////////////////////////////
GPUMesh::GPUMesh(RenderContext* renderer)
{
	m_vertexBuffer = new VertexBuffer(renderer);
	m_indexBuffer = new IndexBuffer(renderer);
}

////////////////////////////////
GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
}

////////////////////////////////
void GPUMesh::CreateFromCPUMesh(const CPUMesh& mesh/*, GPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_IMMUTABLE*/)
{
	const RenderBufferLayout* layout = mesh.GetLayout();
	GUARANTEE_OR_DIE(layout != nullptr, "No Layout desinated");
	int count = mesh.GetVertexCount();
	int sizeOfBuffer = layout->GetStride() * count;
	void* buffer = new char[sizeOfBuffer];

	layout->CopyFromVertexMaster(buffer, mesh.GetVertexBuffer(), count);

	m_vertexBuffer->CreateImmutable(buffer, count, layout);
	m_vertexBuffer->Buffer(buffer, count);
	m_indexBuffer->CreateImmutable(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	m_indexBuffer->Buffer(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	SetDrawCall(mesh.IsUsingIndexBuffer(), mesh.GetElementCount());
	m_layout = layout;

	delete[](char*)buffer;
}

////////////////////////////////
void GPUMesh::CopyFromCPUMesh(const CPUMesh& mesh, GPUMemoryUsage memoryUsage /*= GPU_MEMORY_USAGE_DYNAMIC*/)
{
	UNUSED(memoryUsage);
	const RenderBufferLayout* layout = mesh.GetLayout();
	m_layout = layout;
	GUARANTEE_OR_DIE(layout != nullptr, "No Layout desinated");
	int count = mesh.GetVertexCount();
	int sizeOfBuffer = layout->GetStride() * count;
	void* buffer = new char[sizeOfBuffer];

	m_vertexBuffer->SetLayout(layout);
	layout->CopyFromVertexMaster(buffer, mesh.GetVertexBuffer(), count);
	m_vertexBuffer->Buffer(buffer, count);
	m_indexBuffer->Buffer(mesh.GetIndicesDataBuffer(), mesh.GetIndicesCount());
	SetDrawCall(mesh.IsUsingIndexBuffer(), mesh.GetElementCount());

	delete[](char*)buffer;
}

////////////////////////////////
void GPUMesh::CopyVertexPCUArray(const Vertex_PCU* vertices, size_t count)
{
	m_vertexBuffer->SetLayout(m_layout);
	m_vertexBuffer->Buffer(vertices, (int)count);
	m_count = (int)count;
	m_usingIndexBuffer = false;
}

////////////////////////////////
void GPUMesh::SetDrawCall(bool usingIndexBuffer, int count)
{
	m_usingIndexBuffer = usingIndexBuffer;
	m_count = count;
}


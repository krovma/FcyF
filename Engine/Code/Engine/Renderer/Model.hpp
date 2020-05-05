#pragma once
#include "Engine/Math/Mat4.hpp"

class Material;
class RenderContext;
class GPUMesh;

class Model
{
public:
	Model(RenderContext* renderer, const char* modelPath);
	~Model();
	Model(GPUMesh* mesh);
private:
	GPUMesh* m_mesh = nullptr;
	Material* m_material = nullptr;
	Mat4 m_model;

	///////////////////////////////////////////////
public:
	GPUMesh* GetMesh() const
	{
		return m_mesh;
	}

	void SetMesh(GPUMesh* const mesh)
	{
		m_mesh = mesh;
	}

	Material* GetMaterial() const
	{
		return m_material;
	}

	void SetMaterial(Material* const material)
	{
		m_material = material;
	}

	Mat4 GetModel() const
	{
		return m_model;
	}

	void SetModel(const Mat4& model)
	{
		m_model = model;
	}
};
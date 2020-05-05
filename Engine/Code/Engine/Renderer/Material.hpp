#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Texture2D.hpp"
#include <map>

class Shader;
class RenderContext;
class TextureView2D;
class Sampler;

class Material
{
public:
	static Material* AcquireMaterialFromFile(RenderContext* renderer, const char* path);
public:
	Material(RenderContext* renderer);
	Material(RenderContext* renderer, XmlElement& xml);
	~Material();

	void SetTexture(unsigned int slot, Texture2D* texture);
	void SetTexture(unsigned int slot, const char* textureName);
	
	void SetShader(Shader* shader);

	TextureView2D* GetTextureView(unsigned int slot) const;
	//Sampler* GetSampler(unsigned int slot) const;

	void UseMaterial(RenderContext* renderer);
private:
	Material(const Material &) {}
private:
	Texture2D*		m_textures[NUM_USED_TEXTURES];
	RenderContext*	m_renderer=nullptr;
	Shader*			m_shader=nullptr;

private:
	static std::map<std::string, Material*> s_cachedMaterial;
};

extern Material* g_defaultMaterial;
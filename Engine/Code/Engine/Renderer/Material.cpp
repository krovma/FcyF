#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/Shader.hpp"

STATIC std::map<std::string, Material*> Material::s_cachedMaterial;
Material* g_defaultMaterial = nullptr;

Material* Material::AcquireMaterialFromFile(RenderContext* renderer, const char* path)
{
	if (s_cachedMaterial.find("default") == s_cachedMaterial.end()) {
		s_cachedMaterial["default"] = g_defaultMaterial;
	}
	XmlElement* xml;
	ParseXmlFromFile(xml, path);
	std::string id = ParseXmlAttr(*xml, "id", "default");
	auto found = s_cachedMaterial.find(id);
	if (found != s_cachedMaterial.end()) {
		return found->second;
	}
	Material* created = new Material(renderer, *xml);
	s_cachedMaterial[id] = created;
	return created;
}

Material::Material(RenderContext* renderer)
	:m_renderer(renderer)
{
	for (unsigned int i = 0; i < NUM_USED_TEXTURES; ++i) {
		m_textures[i] = nullptr;
	}
}

Material::~Material()
{
}

Material::Material(RenderContext* renderer, XmlElement& xml)
	: Material(renderer)
{
	std::string shaderName = ParseXmlAttr(xml, "shader", "NO SHADER DATA");
	m_shader = Shader::CreateShaderFromXml(shaderName.c_str(), renderer);

	std::string path;
	const auto diffuse = xml.FirstChildElement("diffuse");
	if (diffuse != nullptr) {
		path = ParseXmlAttr(*diffuse, "src", "White");
		m_textures[TEXTURE_SLOT_DIFFUSE] = renderer->AcquireTextureFromFile(path.c_str());
	}

	const auto normal = xml.FirstChildElement("normal");
	if (normal != nullptr) {
		path = ParseXmlAttr(*normal, "src", "Flat");
		m_textures[TEXTURE_SLOT_NORMAL] = renderer->AcquireTextureFromFile(path.c_str());
	}
	const auto emmisive = xml.FirstChildElement("emmisive");
	if (emmisive != nullptr) {
		path = ParseXmlAttr(*emmisive, "src", "Black");
		m_textures[TEXTURE_SLOT_EMMISIVE] = renderer->AcquireTextureFromFile(path.c_str());
	}
	const auto height = xml.FirstChildElement("height");
	if (height != nullptr) {
		path = ParseXmlAttr(*height, "src", "Black");
		m_textures[TEXTURE_SLOT_HEIGHT] = renderer->AcquireTextureFromFile(path.c_str());
	}
	const auto specular = xml.FirstChildElement("specular");
	if (specular != nullptr) {
		path = ParseXmlAttr(*specular, "src", "White");
		m_textures[TEXTURE_SLOT_SPECULAR] = renderer->AcquireTextureFromFile(path.c_str());
	}
}

void Material::SetTexture(unsigned int slot, Texture2D* texture)
{
	m_textures[slot] = texture;
}

void Material::SetTexture(unsigned int slot, const char* textureName)
{
	m_textures[slot] = m_renderer->AcquireTextureFromFile(textureName);
}

void Material::SetShader(Shader* shader)
{
	m_shader = shader;
}

TextureView2D* Material::GetTextureView(unsigned slot) const
{
	return m_textures[slot]->CreateTextureView();
}

void Material::UseMaterial(RenderContext* renderer)
{
	renderer->BindShader(m_shader);
	for (unsigned int i = 0; i < NUM_USED_TEXTURES; ++i) {
		renderer->BindTextureViewWithSampler(i, m_textures[i]->CreateTextureView());
	}
}

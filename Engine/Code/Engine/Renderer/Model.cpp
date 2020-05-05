#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

Model::Model(RenderContext* renderer, const char* modelPath)
{
	static const char axisFromInt[3] = {'x', 'y', 'z'};
	static const Vec4 basisFromInt[3] = {
		Vec4(1,0,0,0),
		Vec4(0,1,0,0),
		Vec4(0,0,1,0),
	};
	XmlElement* xml;
	ParseXmlFromFile(xml, modelPath);
	bool invert = ParseXmlAttr(*xml, "invert", false);
	std::string meshPath = ParseXmlAttr(*xml, "src", "");
	m_mesh = renderer->AcquireMeshFromFile(meshPath.c_str(), invert);


	float scale = 1.f / ParseXmlAttr(*xml, "downScale", 1.f);
	std::string transform = ParseXmlAttr(*xml, "transform", "x y z");
	const auto splited = Split(transform.c_str());
	if (splited.size() != 3) {
		ERROR_RECOVERABLE(Stringf("Unrecgonized transform specifier in %s\n", modelPath));
	} else {
		for (int i = 0; i < 3; ++i) {
			float reverse = 1.f;
			char axis = 0;
			if (splited[i].length() > 2) {
				ERROR_RECOVERABLE(Stringf("Unrecgonized transform specifier in %s\n", modelPath));
				axis = axisFromInt[i];
				break;
			} else {
				if (splited[i][0] == '-') {
					reverse = -1.f;
					axis = splited[i][1];
				} else {
					axis = splited[i][0];
				}
			}
			if (axis - 'x' > 2 || axis - 'x' < 0) {
				ERROR_RECOVERABLE(Stringf("Unrecgonized transform specifier in %s\n", modelPath));
				axis = axisFromInt[i];
			}
			switch(i) {
			case 0:
				m_model.I = basisFromInt[axis - 'x'] * reverse * scale;
				break;
			case 1:
				m_model.J = basisFromInt[axis - 'x'] * reverse * scale;
				break;
			case 2:
				m_model.K = basisFromInt[axis - 'x'] * reverse * scale;
				break;
			}
		}
		//m_model.K *= 2.f;
	}
	XmlElement* mat = xml->FirstChildElement("material");
	m_material = g_defaultMaterial;
	if (mat) {
		std::string materialPath = ParseXmlAttr(*mat, "src", "default");
		if (materialPath != "default") {
			m_material = Material::AcquireMaterialFromFile(renderer, materialPath.c_str());
		}
	}
	if(ParseXmlAttr(*xml, "tangent", false)) {
		//Gen tangent
	}
}

Model::Model(GPUMesh* mesh)
	: m_mesh(mesh)
	, m_material(g_defaultMaterial)
{
}

////////////////////////////////
Model::~Model()
{
	//delete m_mesh;
}

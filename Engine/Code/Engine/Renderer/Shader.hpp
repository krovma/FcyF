#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include <d3d11.h>
//////////////////////////////////////////////////////////////////////////
struct ID3D11Resource;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D10Blob;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
class RenderBufferLayout;
enum D3D11_FILL_MODE : int;
enum D3D11_CULL_MODE : int;
//////////////////////////////////////////////////////////////////////////

#include "Engine/Renderer/RenderTypes.hpp"

class ShaderStage
{
public:
	friend class Shader;
	ShaderStage();
	~ShaderStage();

	void CreateFromCode(
		const RenderContext* renderContext
		, const std::string& shaderCode
		, ShaderStageType stageType
		, const std::string& filename
		, const char* entrypoint);
	bool IsValid() const { return m_handle != nullptr; }
	ID3D10Blob* GetBytecode() const { return m_bytecode; }
private:
	union
	{
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
	};
	ShaderStageType m_stageType;
	ID3D10Blob* m_bytecode;
};

class Shader
{
public:
	Shader(const RenderContext* renderer);
	~Shader();
	static const char* GetShaderStageEntryName(ShaderStageType stageType);
	static const char* GetShaderModel(ShaderStageType stageType);
	static Shader* CreateShaderFromXml(const std::string& xmlPath, RenderContext* renderer);
public:
	bool CreateShaderFromFile(const std::string& filePath, const char* vertEntry, const char* pixelEntry);
	ID3D11VertexShader* GetVertexShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetVertexBufferLayout() const;
	bool CreateVertexPCULayout(const RenderContext* renderer);
	bool CreateVertexBufferLayout(const RenderContext* renderer, const RenderBufferLayout* layout);
	bool IsValid() const;
	void SetBlendMode(BlendMode blendMode);
	bool UpdateBlendMode(const RenderContext* renderer);
	ID3D11BlendState* GetBlendState() const { return m_blendState; }
	ID3D11DepthStencilState* GetDepthStencilState() const { return m_defaultDepthStencilState; }//
	ID3D11RasterizerState* GetRasterizerState() const { return m_defaultRasterizerState; }//
	void SetDepthStencil(CompareOperator op, bool write);
	bool UpdateDepthStencil(const RenderContext* renderer);
	bool UpdateShaderStates(const RenderContext* renderer);
	bool UpdateRasterizerStates(const RenderContext* renderer);
	void ResetShaderStates();
	void UseState(ID3D11DepthStencilState* depthStencil, ID3D11RasterizerState* rasterize);
private:
	ShaderStage m_vertexShader;
	ShaderStage m_pixelShader;
	ID3D11InputLayout* m_inputLayout = nullptr;
	const RenderBufferLayout* m_layout = nullptr;
	BlendMode m_blendMode = BLEND_MODE_ALPHA;
	bool m_blendModeDirty = true;
	bool m_depthStencilDirty = true;
	bool m_rasterizerDirty = true;
	ID3D11BlendState* m_blendState = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	CompareOperator m_depthStencilOp = COMPARE_LESSEQ;
	bool m_writeDepth = false;

	//ID3D11BlendState* m_defaultBlendState = nullptr;
	ID3D11DepthStencilState* m_defaultDepthStencilState = nullptr;
	ID3D11RasterizerState* m_defaultRasterizerState = nullptr;
private:
	//settings loaded from xml;
	D3D11_CULL_MODE xml_cullMode;// = D3D11_CULL_NONE;
	D3D11_FILL_MODE xml_fillMode;// = D3D11_FILL_SOLID;
	bool xml_FrontCCW = false;

};



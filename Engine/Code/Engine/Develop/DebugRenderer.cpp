#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Develop/DebugRenderer.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Develop/DevConsole.hpp"

STATIC DebugRenderer* DebugRenderer::s = nullptr;

////////////////////////////////
DebugRenderObject::DebugRenderObject(RenderContext* renderer)
{
	m_cpuMesh = new CPUMesh(RenderBufferLayout::AcquireLayoutFor<Vertex_PCU>());
	m_gpuMesh = new GPUMesh(renderer);
	m_gpuMesh->SetLayout(m_cpuMesh->GetLayout());
}


////////////////////////////////
DebugRenderObject::~DebugRenderObject()
{
	delete m_cpuMesh;
	delete m_gpuMesh;
}

////////////////////////////////
STATIC void DebugRenderer::Startup(RenderContext* renderer)
{
	if (s != nullptr) {
		return;
	}
	s = new DebugRenderer(renderer);
}

////////////////////////////////
STATIC void DebugRenderer::Shutdown()
{
	if (s == nullptr) {
		return;
	}
	delete s;
	s = nullptr;
}

////////////////////////////////
void DebugRenderer::BeginFrame()
{

}

////////////////////////////////
STATIC void DebugRenderer::Update(float deltaSeconds)
{
	if (s == nullptr)
		return;
	auto& worldObjects = s->m_WorldObjects;
	auto& screenObjects = s->m_screenObjects;
	auto& messages = s->m_messages;
	// Remove all expired objects
	for (auto iter = worldObjects.begin(); iter != worldObjects.end();) {
		auto& currentObj = *iter;
		currentObj->m_upTime += deltaSeconds;
		if (currentObj->m_lifeTime >= 0.f) {
			if (currentObj->m_upTime > currentObj->m_lifeTime) {
				delete currentObj;
				iter = worldObjects.erase(iter);
				continue;
			}
		}
		++iter;
	}
	for (auto iter = screenObjects.begin(); iter != screenObjects.end();) {
		auto& currentObj = *iter;
		currentObj->m_upTime += deltaSeconds;
		if (currentObj->m_lifeTime >= 0.f) {
			if (currentObj->m_upTime > currentObj->m_lifeTime) {
				delete currentObj;
				iter = screenObjects.erase(iter);
				continue;
			}
		}
		++iter;
	}

	for (auto iter = messages.begin(); iter != messages.end();) 
	{
		auto& msg = *iter;
		msg.m_upTime += deltaSeconds;
		if (msg.m_lifeTime >= 0.f) {
			if (msg.m_upTime > msg.m_lifeTime) {
				iter = messages.erase(iter);
				continue;
			}
		}
		++iter;
	}

	// Update color
	for (auto eachObj : worldObjects) {
		if (eachObj->m_lifeTime < 0.f || !eachObj->m_useGradient) {
			continue;
		}
		CPUMesh* mesh = eachObj->m_cpuMesh;
		Rgba color = eachObj->m_colorGradient.GetColorAt(eachObj->m_upTime / eachObj->m_lifeTime);
		int vertexCount = mesh->GetVertexCount();
		for (int vid = 0; vid < vertexCount; ++vid) {
			mesh->SetVertexColorByIndex(vid, color);
		}
	}

	for (auto eachObj : screenObjects) {
		if (eachObj->m_lifeTime < 0.f) {
			continue;
		}
		CPUMesh* mesh = eachObj->m_cpuMesh;
		Rgba color = eachObj->m_colorGradient.GetColorAt(eachObj->m_upTime / eachObj->m_lifeTime);
		int vertexCount = mesh->GetVertexCount();
		for (int vid = 0; vid < vertexCount; ++vid) {
			mesh->SetVertexColorByIndex(vid, color);
		}
	}
}

////////////////////////////////
STATIC void DebugRenderer::Render(Camera* worldCamera)
{
	if (s == nullptr)
		return;
	if (!s->m_isRendering)
		return;
	RenderContext* renderer = s->m_renderer;
	Shader* shader = s->m_shader;
	renderer->BindShader(shader);
	
	// On World Objects
	shader->ResetShaderStates();
	RenderTargetView* renderTarget = renderer->GetFrameColorTarget();
	worldCamera->SetRenderTarget(renderTarget);
	DepthStencilTargetView* dstTarget = renderer->GetFrameDepthStencilTarget();
	worldCamera->SetDepthStencilTarget(dstTarget);

	renderer->BeginCamera(*worldCamera);

	ConstantBuffer* rendererModel = renderer->GetModelBuffer();
	Mat4 cameraRotation = worldCamera->GetCameraModel();
	cameraRotation[Tx] = 0;
	cameraRotation[Ty] = 0;
	cameraRotation[Tz] = 0;
	cameraRotation[Tw] = 1;
	std::vector<DebugRenderObject*>& worldObjects = s->m_WorldObjects;
	for (auto eachObject : worldObjects) {
		eachObject->m_gpuMesh->CopyFromCPUMesh(*(eachObject->m_cpuMesh));
	}
	// X-ray Pre render
	shader->SetBlendMode(BLEND_MODE_ADDITIVE);
	for (auto eachObject : worldObjects) {
		if (eachObject->m_renderMode != DEBUG_RENDER_XRAY) {
			continue;
		}
		//Vec3 newOrientation = cameraPosition - eachObject->m_position;
		Mat4 model = Mat4::MakeTranslate3D(eachObject->m_position);
		if (eachObject->m_isBillboard) {
			model *= cameraRotation;
			/*DebuggerPrintf("%g %g %g\n", roatation.x, roatation.y, roatation.z);*/
		}
		renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
		rendererModel->Buffer(&model, sizeof(model));
		renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
		shader->UseState(s->m_dsXray, s->GetRasterizerState(eachObject->m_rasterizeMode));
		renderer->DrawMesh(*(eachObject->m_gpuMesh));
	}
	// Normal rendering
	shader->SetBlendMode(BLEND_MODE_ALPHA);
	for (auto eachObject : worldObjects) {
		if (eachObject->m_isTransparent) {
			continue;
		}
		//Vec3 newOrientation = cameraPosition - eachObject->m_position;
		Mat4 model = Mat4::MakeTranslate3D(eachObject->m_position);
		if (eachObject->m_isBillboard) {
			model *= cameraRotation;
			/*DebuggerPrintf("%g %g %g\n", roatation.x, roatation.y, roatation.z);*/
		}
		renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
		rendererModel->Buffer(&model, sizeof(model));
		renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
		shader->UseState(s->m_dsUseDepth, s->GetRasterizerState(eachObject->m_rasterizeMode));
		renderer->DrawMesh(*(eachObject->m_gpuMesh));
	}
	// AlwaysMode After renfering
	for (auto eachObject : worldObjects) {
		if (eachObject->m_renderMode == DEBUG_RENDER_ALWAYS || eachObject->m_isTransparent == true) {
			Mat4 model = Mat4::MakeTranslate3D(eachObject->m_position);
			if (eachObject->m_isBillboard) {
				model *= cameraRotation;
				/*DebuggerPrintf("%g %g %g\n", roatation.x, roatation.y, roatation.z);*/
			}
			renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
			rendererModel->Buffer(&model, sizeof(model));
			renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
			shader->UseState(
				eachObject->m_isTransparent ? s->m_dsTransparent : s->m_dsAlways
				, s->GetRasterizerState(eachObject->m_rasterizeMode));
			renderer->DrawMesh(*(eachObject->m_gpuMesh));
		}
	}

	renderer->EndCamera(*worldCamera);

	//////////////////////////////////////////////////////////////////////////
	//On Screen Objects
	shader->ResetShaderStates();
	Camera* screenCamera = s->m_screenCamera;
	screenCamera->SetRenderTarget(renderTarget);
	screenCamera->SetDepthStencilTarget(dstTarget);
	renderer->BeginCamera(*screenCamera);
	rendererModel->Buffer(&Mat4::Identity, sizeof(Mat4));
	for (auto eachObject : s->m_screenObjects) {
		eachObject->m_gpuMesh->CopyFromCPUMesh(*(eachObject->m_cpuMesh));
		shader->UseState(s->m_dsAlways, s->GetRasterizerState(DEBUG_RENDER_FILL));
		renderer->BindConstantBuffer(CONSTANT_SLOT_MODEL, rendererModel);
		renderer->BindTextureViewWithSampler(0, eachObject->m_texture);
		renderer->DrawMesh(*(eachObject->m_gpuMesh));
	}

	//////////////////////////////////////////////////////////////////////////
	constexpr float linehight = 20.f;//Pixel
	BitmapFont* font = DevConsole::s_consoleFont;
	float y = linehight;
	float screenHeight = renderTarget->GetHeight();
	std::vector<Vertex_PCU> verts;
	for (int i = (int)s->m_messages.size() - 1; i >= 0; --i) {
		auto& msg = s->m_messages[i];
		font->AddVertsForText2D(verts, Vec2(0, y), linehight, msg.m_text, 
			msg.m_lifeTime > 0.f ?
			msg.m_colorGradient.GetColorAt(msg.m_upTime / msg.m_lifeTime)
			: msg.m_colorGradient.GetColorAt(-1.f));
		y += linehight;
		if (y >= screenHeight) {
			break;
		}
	}
	renderer->BindTextureViewWithSampler(0, font->GetFontTexture());
	renderer->DrawVertexArray(verts.size(), verts);
	renderer->BindTextureViewWithSampler(0, nullptr);
	renderer->EndCamera(*screenCamera);
}

////////////////////////////////
void DebugRenderer::EndFrame()
{

}

////////////////////////////////
void DebugRenderer::Clear()
{
	if (!s)
		return;
	for (auto each : s->m_WorldObjects)
		delete each;
	for (auto each : s->m_screenObjects)
		delete each;
	s->m_WorldObjects.clear();
	s->m_screenObjects.clear();
}

////////////////////////////////
void DebugRenderer::ToggleRendering(bool isRendering)
{
	if (s == nullptr)
		return;
	s->m_isRendering = isRendering;
}

////////////////////////////////
void DebugRenderer::ToggleRendering()
{
	if (s == nullptr)
		return;
	s->m_isRendering = !s->m_isRendering;
}
#include "Engine/Develop/Profile.hpp"
////////////////////////////////
DebugRenderObject* DebugRenderer::DrawPoint3D(const Vec3& position, float size, float time/*=-1.f*/, const ColorGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	PROFILE_SCOPE(__FUNCTION__);
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* point = new DebugRenderObject(s->m_renderer);
	point->m_position = position;
	point->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	point->m_cpuMesh->AddAABB2ToMesh(AABB2::MakeAABB2OfSize(size, size));
	point->m_lifeTime = time;
	point->m_colorGradient = colorGradient;
	point->m_isBillboard = true;
	s->m_WorldObjects.push_back(point);
	return point;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawLine3D(const Vec3& start, const Vec3& end, float thickness, float time/*=-1.f*/, const ColorGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* line = new DebugRenderObject(s->m_renderer);
	line->m_position = start;
	line->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	line->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, end - start, thickness, 4, 3);
	line->m_lifeTime = time;
	line->m_colorGradient = colorGradient;
	line->m_isBillboard = false;
	s->m_WorldObjects.push_back(line);
	return line;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawArrow3D(const Vec3& start, const Vec3& end, float thickness, float headSize, float time/*=-1.f*/, const ColorGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* arrow = new DebugRenderObject(s->m_renderer);
	arrow->m_position = start;
	arrow->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	Vec3 shaftDisp = end - start;
	shaftDisp.SetLength(shaftDisp.GetLength() - headSize);
	arrow->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, shaftDisp, thickness, 4, 3);
	arrow->m_cpuMesh->AddConeToMesh(shaftDisp, headSize * 0.4f, end - start);
	arrow->m_lifeTime = time;
	arrow->m_colorGradient = colorGradient;
	arrow->m_isBillboard = false;
	s->m_WorldObjects.push_back(arrow);
	return arrow;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawAABB2In3D(const AABB2& shape, float zBias, TextureView2D* texture /*= nullptr*/, float time /*= -1.f*/, const ColorGradient colorGradient /*= ColorGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* aabb2 = new DebugRenderObject(s->m_renderer);
	aabb2->m_position = Vec3(shape.GetCenter(), zBias);
	aabb2->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	aabb2->m_cpuMesh->AddAABB2ToMesh(AABB2(-shape.GetExtend(), shape.GetExtend()));
	aabb2->m_lifeTime = time;
	aabb2->m_colorGradient = colorGradient;
	aabb2->m_texture = texture;
	s->m_WorldObjects.push_back(aabb2);
	return aabb2;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawBillboardQuad(const Vec3& center, const AABB2& quadShape, TextureView2D* texture/*=nullptr*/, float time/*=-1.f*/, const ColorGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* quad = new DebugRenderObject(s->m_renderer);
	quad->m_position = center;
	quad->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	quad->m_cpuMesh->AddAABB2ToMesh(quadShape);
	quad->m_lifeTime = time;
	quad->m_colorGradient = colorGradient;
	quad->m_texture = texture;
	quad->m_isBillboard = true;
	s->m_WorldObjects.push_back(quad);
	return quad;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawBillboardText(const Vec3& center, const AABB2& textBoxShape, const BitmapFont* font, float cellHeight, const std::string& text, float time/*=-1.f*/, const Vec2& alignment, const ColorGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	std::vector<Vertex_PCU> verts;
	font->AddTextInBox(verts, text, textBoxShape, alignment, cellHeight, colorGradient.GetColorAt(0.f));
	DebugRenderObject* btext = new DebugRenderObject(s->m_renderer);
	CPUMesh* cpuMesh = btext->m_cpuMesh;
	for (auto& each : verts) {
		cpuMesh->SetBrushColor(each.m_color);
		cpuMesh->SetBrushUV(each.m_uvTexCoords);
		cpuMesh->AddVertexAndIndex(each.m_position);
	}
	btext->m_position = center;
	btext->m_lifeTime = time;
	btext->m_colorGradient = colorGradient;
	btext->m_texture = font->GetFontTexture();
	btext->m_isBillboard = true;
	btext->m_isTransparent = true;
	s->m_WorldObjects.push_back(btext);
	return btext;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawWireBall(const Vec3& center, float radius, float time/*=-1.f*/, const ColorGradient colorGradient/*=RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* ball = new DebugRenderObject(s->m_renderer);
	ball->m_position = center;
	ball->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	ball->m_cpuMesh->AddUVSphereToMesh(center, radius, 8, 8);
	ball->m_lifeTime = time;
	ball->m_colorGradient = colorGradient;
	ball->m_isBillboard = false;
	ball->m_rasterizeMode = DEBUG_RENDER_WIREFRAME;
	ball->m_renderMode = DEBUG_RENDER_DEPTH;
	s->m_WorldObjects.push_back(ball);
	return ball;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawBasis(Mat4 basis, float thickness, float headSize, float time /*= -1.f*/)
{
	if (s == nullptr)
		return nullptr;
	basis *= 1.f / basis[Tw];
	Vec3 position = basis.GetT().XYZ();

	DebugRenderObject* dbas = new DebugRenderObject(s->m_renderer);
	dbas->m_position = position;
	dbas->m_cpuMesh->SetBrushColor(Rgba::RED);
	Vec3 shaftDisp = basis.GetI().XYZ();
	shaftDisp.SetLength(shaftDisp.GetLength() - headSize);
	dbas->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, shaftDisp, thickness, 8, 3);
	dbas->m_cpuMesh->AddConeToMesh(shaftDisp, headSize * 0.4f, shaftDisp * (1+headSize));
	dbas->m_cpuMesh->SetBrushColor(Rgba::GREEN);
	shaftDisp = basis.GetJ().XYZ();
	shaftDisp.SetLength(shaftDisp.GetLength() - headSize);
	dbas->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, shaftDisp, thickness, 8, 3);
	dbas->m_cpuMesh->AddConeToMesh(shaftDisp, headSize * 0.4f, shaftDisp * (1+headSize));
	dbas->m_cpuMesh->SetBrushColor(Rgba::BLUE);
	shaftDisp = basis.GetK().XYZ();
	shaftDisp.SetLength(shaftDisp.GetLength() - headSize);
	dbas->m_cpuMesh->AddCylinderToMesh(Vec3::ZERO, shaftDisp, thickness, 8, 3);
	dbas->m_cpuMesh->AddConeToMesh(shaftDisp, headSize * 0.4f, shaftDisp * (1+headSize));

	dbas->m_lifeTime = time;
	dbas->m_useGradient = false;
	dbas->m_isBillboard = false;
	s->m_WorldObjects.push_back(dbas);

	return dbas;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawPoint2D(const Vec2& position, float size, float time /*= -1.f*/, const ColorGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* point = new DebugRenderObject(s->m_renderer);
	point->m_position = Vec3(position, 0.f);
	point->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	point->m_cpuMesh->AddAABB2ToMesh(AABB2::MakeAABB2OfSize(size, size) + position);
	point->m_lifeTime = time;
	point->m_colorGradient = colorGradient;
	s->m_screenObjects.push_back(point);
	return point;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawLine2D(const Vec2& start, const Vec2& end, float thickness, float time /*= -1.f*/, const ColorGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* line = new DebugRenderObject(s->m_renderer);
	CPUMesh* cpuMesh = line->m_cpuMesh;
	std::vector<Vertex_PCU> verts;
	AddVerticesOfLine2D(verts, start, end, thickness, colorGradient.GetColorAt(0.f));
	for (auto& each : verts) {
		cpuMesh->SetBrushColor(each.m_color);
		cpuMesh->SetBrushUV(each.m_uvTexCoords);
		cpuMesh->AddVertexAndIndex(each.m_position);
	}
	line->m_lifeTime = time;
	line->m_colorGradient = colorGradient;
	s->m_screenObjects.push_back(line);
	return line;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawQuad2D(const AABB2& quad, TextureView2D* texture /*= nullptr*/, float time /*= -1.f*/, const ColorGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	DebugRenderObject* box = new DebugRenderObject(s->m_renderer);
	box->m_cpuMesh->SetBrushColor(colorGradient.GetColorAt(0.f));
	box->m_cpuMesh->AddAABB2ToMesh(quad);
	box->m_lifeTime = time;
	box->m_colorGradient = colorGradient;
	box->m_texture = texture;
	s->m_screenObjects.push_back(box);
	return box;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawText2D(const AABB2& textBox, const BitmapFont* font, float cellHeight, const std::string& text, float time /*= -1.f*/, const Vec2& alignment /*= BitmapFont::ALIGNMENT_CENTER*/, const ColorGradient colorGradient /*= RgbaGradient::WHITE_NOGRADIENT*/)
{
	if (s == nullptr)
		return nullptr;
	std::vector<Vertex_PCU> verts;
	font->AddTextInBox(verts, text, textBox, alignment, cellHeight, colorGradient.GetColorAt(0.f));
	DebugRenderObject* btext = new DebugRenderObject(s->m_renderer);
	CPUMesh* cpuMesh = btext->m_cpuMesh;
	for (auto& each : verts) {
		cpuMesh->SetBrushColor(each.m_color);
		cpuMesh->SetBrushUV(each.m_uvTexCoords);
		cpuMesh->AddVertexAndIndex(each.m_position);
	}
	btext->m_lifeTime = time;
	btext->m_colorGradient = colorGradient;
	btext->m_texture = font->GetFontTexture();
	s->m_screenObjects.push_back(btext);
	return btext;
}

////////////////////////////////
DebugRenderObject* DebugRenderer::DrawCameraBasisOnScreen(const Camera& camera, float time /*= -1.f*/)
{
	Mat4 model = camera.GetCameraModel();
	static constexpr float arrowSize = 50.f;
	static Vec3 start = Vec3(arrowSize, arrowSize, 0);
	Vec3 camera_i = model.GetI().XYZ().GetNormalized() * arrowSize;
	Vec3 camera_j = model.GetJ().XYZ().GetNormalized() * arrowSize;
	Vec3 camera_k = model.GetK().XYZ().GetNormalized() * arrowSize;

	DebugRenderObject* basis = new DebugRenderObject(s->m_renderer);
	basis->m_renderMode = DEBUG_RENDER_ALWAYS;
	CPUMesh* mesh = basis->m_cpuMesh;
	mesh->SetBrushColor(Rgba::RED);
	mesh->AddCylinderToMesh(start, camera_i + start, 1.f);
	mesh->AddConeToMesh(camera_i + start, 5.f, camera_i * 1.1f + start);
	mesh->SetBrushColor(Rgba::GREEN);
	mesh->AddCylinderToMesh(start, camera_j + start, 1.f);
	mesh->AddConeToMesh(camera_j + start, 5.f, camera_j * 1.1f + start);
	mesh->SetBrushColor(Rgba::BLUE);
	mesh->AddCylinderToMesh(start, camera_k + start, 1.f);
	mesh->AddConeToMesh(camera_k + start, 5.f, camera_k * 1.1f + start);

	basis->m_lifeTime = time;
	s->m_screenObjects.push_back(basis);
	return basis;
}

////////////////////////////////
void DebugRenderer::Log(const std::string& text, float time /*= 2.f*/, const ColorGradient colorGradient/*=ColorGradient::FADEOUT*/)
{
	DebugOutputMessage msg;
	msg.m_text = text;
	msg.m_lifeTime = time;
	msg.m_colorGradient = colorGradient;
	s->m_messages.push_back(msg);
}

////////////////////////////////
void DebugRenderer::ClearLog()
{
	s->m_messages.clear();
}

////////////////////////////////
DebugRenderer::DebugRenderer(RenderContext* renderer)
	:m_renderer(renderer)
{
	// create default depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and comprae is always)
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = GetD3DComparisonFunc(COMPARE_GREATEREQ);
	// Stencil - just use defaults for now; 
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	D3D11_DEPTH_STENCILOP_DESC opDesc;
	memset(&opDesc, 0, sizeof(opDesc));
	opDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
	opDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
	opDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
	opDesc.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;      // function to test against
	// can have different rules setup for front and backface
	depthStencilDesc.FrontFace = opDesc;
	depthStencilDesc.BackFace = opDesc;
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsUseDepth);
	
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsTransparent);
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthFunc = GetD3DComparisonFunc(COMPARE_ALWAYS);
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsXray);
	depthStencilDesc.StencilEnable = FALSE;
	renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_dsAlways);

	
	

	// create default rastrization state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	memset(&rasterizerDesc, 0, sizeof(rasterizerDesc));
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;
	renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_rsNormal);

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, &m_rsWireframe);

	m_shader = Shader::CreateShaderFromXml("Data/Shaders/unlit.shader.xml", g_theRenderer);

	IntVec2 res = renderer->GetResolution();
	m_screenCamera = new Camera(Vec2(0, 0), Vec2((float)res.x, (float)res.y));
	m_screenCamera->SetOrthoView(Vec2(0, 0), Vec2((float)res.x, (float)res.y), 100.f, -100.f);
}

////////////////////////////////
DebugRenderer::~DebugRenderer()
{
	DX_SAFE_RELEASE(m_rsWireframe);
	DX_SAFE_RELEASE(m_rsNormal);
	DX_SAFE_RELEASE(m_dsTransparent);
	DX_SAFE_RELEASE(m_dsXray);
	DX_SAFE_RELEASE(m_dsUseDepth);
	DX_SAFE_RELEASE(m_dsAlways);
	delete m_screenCamera;
	for (auto eachObj : m_WorldObjects) {
		delete eachObj;
	}
	m_WorldObjects.clear();
	for (auto eachObj : m_screenObjects) {
		delete eachObj;
	}
	m_screenObjects.clear();
	m_messages.clear();

}
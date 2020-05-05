#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_VIDEO )
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Develop/DevConsole.hpp"
#include "Engine/Develop/DebugRenderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Develop/Memory.hpp"
#include "Engine/Develop/Log.hpp"
#include "Engine/Develop/Profile.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "ThirdParty/imgui/imgui.h"
#include <mutex>
#include <algorithm>
static std::mutex lock;
//////////////////////////////////////////////////////////////////////////
STATIC BitmapFont* DevConsole::s_consoleFont = nullptr;

////////////////////////////////
STATIC bool DevConsole::Command_Test(EventParam& param)
{
	g_theConsole->Print(Stringf("<Test> Args:\n"));
	param.DebugPrintToConsole(g_theConsole);
	return true;
}

////////////////////////////////
STATIC bool DevConsole::Command_ClearScreen(EventParam& param)
{
	UNUSED(param);
	g_theConsole->Clear();
	return true;
}

////////////////////////////////
bool DevConsole::Command_Exit(EventParam& param)
{
	UNUSED(param);
	g_theConsole->SetConsoleMode(CONSOLE_OFF);
	return true;
}

////////////////////////////////
bool DevConsole::Command_Help(EventParam& param)
{
	UNUSED(param);
	std::vector<std::string> allEvent = g_Event->GetAllEventNames();
	for (auto eachEvent : allEvent) {
		g_theConsole->Print(eachEvent);
	}
	return true;
}

////////////////////////////////
STATIC bool DevConsole::Command_ClearHistory(EventParam& param)
{
	UNUSED(param);
	g_theConsole->ClearHistroy();
	return true;
}

////////////////////////////////
bool DevConsole::Command_ClearDebugRender(EventParam& param)
{
	UNUSED(param);
	DebugRenderer::Clear();
	return true;
}

////////////////////////////////
bool DevConsole::Command_ToggleDebugRender(EventParam& param)
{
	std::string p;
	p = param.GetString("on", "uKr8tUa2uU");
	if (p != "uKr8tUa2uU") {
		DebugRenderer::ToggleRendering(true);
		return true;
	}
	p = param.GetString("off", "uKr8tUa2uU");
	if (p != "uKr8tUa2uU") {
		DebugRenderer::ToggleRendering(false);
		return true;
	}
	DebugRenderer::ToggleRendering();
	return true;
}

////////////////////////////////
DevConsole::DevConsole(RenderContext* renderer, int line, int column)
	: m_renderer(renderer)
	, m_layout(column, line)
	, m_consoleRect(0, 0,
		s_consoleFont->GetGlyphAspect('m') * column, (float) line)
	, m_consoleCamera(m_consoleRect.Min, m_consoleRect.Max)
{
}

////////////////////////////////
void DevConsole::Startup()
{
	m_frameFromStartup = 0;
	g_Event->SubscribeEventCallback("test", DevConsole::Command_Test);
	g_Event->SubscribeEventCallback("cls", DevConsole::Command_ClearScreen);
	g_Event->SubscribeEventCallback("exit", DevConsole::Command_Exit);
	g_Event->SubscribeEventCallback("help", DevConsole::Command_Help);
	g_Event->SubscribeEventCallback("clearhistory", DevConsole::Command_ClearHistory);
	g_Event->SubscribeEventCallback("debugdraw", DevConsole::Command_ToggleDebugRender);
	g_Event->SubscribeEventCallback("debugclear", DevConsole::Command_ClearDebugRender);

	g_Event->SubscribeEventCallback("member", this, &DevConsole::_TesterForMemberFunc);

}

////////////////////////////////
void DevConsole::BeginFrame()
{

}

////////////////////////////////
void DevConsole::Update(float deltaSecond)
{
	static float upTime = 0.f;
	upTime += deltaSecond;
	if (upTime > 0.3f) {
		if (m_currentCaretColor == Rgba::TRANSPARENT_WHITE) {
			m_currentCaretColor = Rgba::WHITE;
		} else {
			m_currentCaretColor = Rgba::TRANSPARENT_WHITE;
		}
		upTime = 0;
	}
	m_isOpenLastFrame = (m_mode == CONSOLE_PASSIVE);
}

////////////////////////////////
void DevConsole::EndFrame()
{
	++m_frameFromStartup;
}

////////////////////////////////
void DevConsole::Shutdown()
{

}

////////////////////////////////
void DevConsole::RenderConsoleUI()
{
	static int selectedIdx = 0;
	static bool paused = false;
	PROFILE_SCOPE("DevConsole::RenderProfilerUI");
	std::vector<float> frameTimes { GetFrameTimeList() };
	ImGui::Begin("Profiler", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Frame Time");
	//ImGui::SetNextWindowContentWidth(1024.f);
	ImGui::SetNextItemWidth(1024.f);
	//ImGui::PushID()
	ImGui::PlotLines("",
		[](void* data, int idx) {
			return ((float*)data)[idx];
		},
		frameTimes.data(),
		frameTimes.size(),
		0,
		"",
		1000.f,
		*std::max_element(frameTimes.begin(), frameTimes.end()),
		{0, 300}
	);
	if (ImGui::IsItemClicked(0)) {
		ProfilePause();
		ImVec2 mouse = ImGui::GetMousePos();
		ImVec2 min = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		const float t = Clamp((mouse.x - min.x) / (max.x - min.x), 0.0f, 0.9999f);
		selectedIdx = (int)(t * (float)frameTimes.size());
		paused = true;
		//Log("Imgui", "clicked");
		RunCommandString(Stringf("report f=%d", frameTimes.size() - selectedIdx));
	} else if (ImGui::IsItemClicked(1)) {
		ProfileResume();
		paused = false;
	}
	if (paused) {
		ImGui::Text("Frame# %d : %g us", selectedIdx, frameTimes[selectedIdx]);
	}
	ImGui::End();
}

////////////////////////////////
void DevConsole::SetConsoleMode(ConsoleMode mode)
{
	m_mode = mode;
// 	if (mode == CONSOLE_OFF) {
// 		ProfileResume();
// 	} else {
// 		ProfilePause();
// 	}
}

////////////////////////////////
void DevConsole::Print(std::string text, const Rgba& color)
{
	std::scoped_lock _(lock);
	ConsoleOutputItem item;
	item.m_message = text;
	item.m_color = color;
	item.m_messageFrame = m_frameFromStartup;
	m_output.push_back(item);
}

////////////////////////////////
void DevConsole::Input(char input)
{
	if (!m_isOpenLastFrame)
		return;
	if (std::isprint(input)) {
		IntVec2 sliceBeforeCaret(0, m_caretPos);
		IntVec2 sliceAfterCaret(
			m_caretPos,
			(int)m_inputBuffer.length()
		);
		m_inputBuffer = m_inputBuffer.substr(sliceBeforeCaret.x, sliceBeforeCaret.y)
			+ input + m_inputBuffer.substr(sliceAfterCaret.x, sliceAfterCaret.y);
		++m_caretPos;
	}
}

////////////////////////////////
void DevConsole::KeyPress(ConsoleKeys key)
{
	switch (key) {
	case CONSOLE_ESC: {
		if (m_inputBuffer.empty()) {
			//ProfileResume();
			SetConsoleMode(CONSOLE_OFF);
		} else {
			m_inputBuffer.clear();
			m_caretPos = 0;
		}
		break;
	}
	case CONSOLE_ENTER: {
		Print(m_inputBuffer, Rgba::GRAY);
		if (m_history.size() > 0) {
			if (m_history[m_history.size() - 1] != m_inputBuffer
			&&	m_history[0] != m_inputBuffer) {
				m_history.push_back(m_inputBuffer);
			}
		} else {
			m_history.push_back(m_inputBuffer);
		}
		RunCommandString(m_inputBuffer);
		m_inputBuffer.clear();
		m_caretPos = 0;
		m_historyCursor = 0;
		break;
	}
	case CONSOLE_BACKSPACE: {
		if (m_inputBuffer.empty() || m_caretPos == 0)
			break;
		IntVec2 sliceBeforeCaret(0, m_caretPos - 1);
		IntVec2 sliceAfterCaret(
			Clamp(m_caretPos, 0, (int)m_inputBuffer.length()),
			(int)m_inputBuffer.length()
		);
		m_inputBuffer = m_inputBuffer.substr(sliceBeforeCaret.x, sliceBeforeCaret.y)
			+ m_inputBuffer.substr(sliceAfterCaret.x, sliceAfterCaret.y);
		--m_caretPos;
		break;
		}
	case CONSOLE_DELETE: {
		IntVec2 sliceBeforeCaret(0, m_caretPos);
		IntVec2 sliceAfterCaret(
			Clamp(m_caretPos + 1, 0, (int)m_inputBuffer.length()),
			(int)m_inputBuffer.length()
		);
		m_inputBuffer = m_inputBuffer.substr(sliceBeforeCaret.x, sliceBeforeCaret.y)
			+ m_inputBuffer.substr(sliceAfterCaret.x, sliceAfterCaret.y);
		break;
		}
	case CONSOLE_LEFT: {
		--m_caretPos;
		m_caretPos = Clamp(m_caretPos, 0, (int)m_inputBuffer.length());
		break;
	}
	case CONSOLE_RIGHT: {
		++m_caretPos;
		m_caretPos = Clamp(m_caretPos, 0, (int)m_inputBuffer.length());
		break;
	}
	case CONSOLE_UP: {
		--m_historyCursor;
		if (m_historyCursor < 0) {
			m_historyCursor = (int)m_history.size() - 1;
		}
		if (m_historyCursor >= 0) {
			m_inputBuffer = m_history[m_historyCursor];
			m_caretPos = (int)m_inputBuffer.size();
		}
		break;
	}
	case CONSOLE_DOWN: {
		++m_historyCursor;
		if (m_historyCursor > m_history.size()) {
			m_historyCursor = 0;
		}
		if (m_historyCursor < m_history.size()) {
			m_inputBuffer = m_history[m_historyCursor];
			m_caretPos = (int)m_inputBuffer.size();
		}
		break;
	}

	}
}

////////////////////////////////
void DevConsole::Clear()
{
	m_output.clear();
}

////////////////////////////////
void DevConsole::RunCommandString(std::string cmd)
{
	//Log("Engine", "Run command %s", cmd.c_str());
	std::vector<std::string> allCmds = Split(cmd.c_str(), '\n');
	for (auto& eachCmd : allCmds) {
		std::vector<std::string> argv = Split(eachCmd.c_str(), ' ');
		int argc = (int)argv.size();
		if (argc > 0) {
			std::string cmdName = argv[0];
			NamedStrings args;
			for (int i = 1; i < argc; ++i) {
				std::vector<std::string> keyValuePair = Split(argv[i].c_str(), '=');
				std::string key = "null";
				std::string value = "null";
				if (keyValuePair.size() > 0) {
					key = keyValuePair[0];
					if (keyValuePair.size() > 1) {
						value = keyValuePair[1];
					}
				}
				args.Set(key, value);
			}
			int numTriggered = g_Event->Trigger(cmdName, args);
			if (numTriggered <= 0) {
				Print(Stringf("%s is not a valid command", cmdName.c_str()), Rgba::RED);
			}
		}
	}
}

////////////////////////////////
void DevConsole::ClearHistroy()
{
	m_history.clear();
}

bool DevConsole::_TesterForMemberFunc(EventParam& param)
{
	{
		g_theConsole->Print(Stringf("<MemberTest> Args:\n"));
		param.DebugPrintToConsole(g_theConsole);
		return true;
	}
}

////////////////////////////////
void DevConsole::RenderConsole() 
{
	PROFILE_SCOPE(__FUNCTION__);
	if (m_mode == CONSOLE_OFF) {
		return;
	}

	std::vector<Vertex_PCU> verts;
	if (m_mode == CONSOLE_PASSIVE) {
		RenderTargetView* renderTarget = m_renderer->GetFrameColorTarget();
		m_consoleCamera.SetRenderTarget(renderTarget);
		DepthStencilTargetView* dst = m_renderer->GetFrameDepthStencilTarget();
		m_consoleCamera.SetDepthStencilTarget(dst);
		//m_renderer->ClearDepthStencilTarget(0.0f);
		m_renderer->BeginCamera(m_consoleCamera);
		m_renderer->GetModelBuffer()->Buffer(&Mat4::Identity, sizeof(Mat4));

		AddVerticesOfAABB2D(verts, m_consoleRect, Rgba(0,0,0, 0.6f));
		m_renderer->BindTextureViewWithSampler(0, nullptr);
		m_renderer->DrawVertexArray(verts.size(), verts);
		verts.clear();
		int numPrintedLines = 0;
		for (int line = (int) m_output.size() - 1; line >= 0; --line) {
			if (numPrintedLines >= m_layout.y) {
				break;
			}
			Vec2 min(0, (float)numPrintedLines + 1);
			s_consoleFont->AddVertsForText2D(verts, min, 1.f, 
				m_output[line].m_message, 
				m_output[line].m_color);
			++numPrintedLines;
			//DebuggerPrintf(Stringf("%d line\n", numPrintedLines).c_str());
		}
		Vec2 min(0, 0);
		s_consoleFont->AddVertsForText2D(verts, min, 1.f,
			"> " + m_inputBuffer
		);
		m_renderer->BindTextureViewWithSampler(0, s_consoleFont->GetFontTexture());
		m_renderer->DrawVertexArray(verts.size(), verts);
		//Rendering Memory Text
		verts.clear();
		std::string mem = Stringf("P:%4d mem: %6d [%12.12s]", GetTotalProfiledFrames(), GetLiveAllocationCount(),
			GetByteSizeString(GetLiveAllocationSize()).c_str()
			);
		min.x = (float)(m_layout.x - (int)mem.length());
		min.y = (float)(m_layout.y - 1);
		s_consoleFont->AddVertsForText2D(verts, min, 1.f, mem);
		m_renderer->DrawVertexArray(verts.size(), verts);

		verts.clear();
		m_renderer->BindTextureViewWithSampler(0, nullptr);
		AddVerticesOfAABB2D(verts,
			AABB2(m_caretPos + 2.f, 0, m_caretPos + 2.5f, 1),
			m_currentCaretColor
		);
		m_renderer->DrawVertexArray(verts.size(), verts);


		m_renderer->BindTextureViewWithSampler(0, nullptr);
		
		RenderConsoleUI();

		m_renderer->EndCamera(m_consoleCamera);
		
	}
}

#endif
#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Event/EventSystem.hpp"
#include <string>
#include <vector>


enum ConsoleMode
{
	CONSOLE_OFF = 0,
	//CONSOLE_PASSIVE_CONSISE,
	CONSOLE_PASSIVE,
	//CONSOLE_PROMPT,
	//CONSOLE_CONSISE,
	CONSOLE_FULL,
	CONSOLE_ONLY,

	NUM_CONSOLE_MODE
};

class ConsoleOutputItem
{
	friend class DevConsole;

private:
	std::string m_message;
	Rgba m_color;
	int m_messageFrame;
};

enum ConsoleKeys
{
	CONSOLE_ENTER,
	CONSOLE_BACKSPACE,
	CONSOLE_DELETE,
	CONSOLE_LEFT,
	CONSOLE_RIGHT,
	CONSOLE_UP,
	CONSOLE_DOWN,
	CONSOLE_ESC
};

class DevConsole
{
public:
	static bool Command_Test(EventParam& param);
	static bool Command_ClearScreen(EventParam& param);
	static bool Command_Exit(EventParam& param);
	static bool Command_Help(EventParam& param);
	static bool Command_ClearHistory(EventParam& param);
	static bool Command_ClearDebugRender(EventParam& param);
	static bool Command_ToggleDebugRender(EventParam& param);
public:
	static BitmapFont* s_consoleFont;

public:
	DevConsole(RenderContext* renderer,int line, int column);
	
	void Startup();
	void BeginFrame();
	void Update(float deltaSecond);
	void RenderConsole();
	void EndFrame();
	void Shutdown();
	void RenderConsoleUI();

	void SetConsoleMode(ConsoleMode mode);
	ConsoleMode GetConsoleMode() const { return m_mode; }
	void Print(std::string text, const Rgba& color=Rgba::WHITE);
	void Input(char input);
	void KeyPress(ConsoleKeys key);
	void Clear();
	void RunCommandString(std::string cmd);
	void ClearHistroy();

	bool _TesterForMemberFunc(EventParam& param);

private:
	ConsoleMode m_mode = CONSOLE_OFF;
	IntVec2 m_layout;
	AABB2 m_consoleRect;
	std::vector<ConsoleOutputItem> m_output;
	RenderContext* m_renderer;
	Camera m_consoleCamera;
	int m_frameFromStartup;
	std::string m_inputBuffer;
	bool m_isOpenLastFrame = false;
	Rgba m_currentCaretColor = Rgba::TRANSPARENT_WHITE;
	int m_caretPos = 0;
	std::vector<std::string> m_history;
	int m_historyCursor = 0;

};

extern DevConsole* g_theConsole;
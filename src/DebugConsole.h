//
// DebugConsole.h - Debug console hooked up with AngelScript and ImGui.
// Copyright (C) Sebastian Nordgren 
// October 12th 2015
//

#pragma once

#include "IDebugPanel.h"

struct ImGuiInputTextCallbackData;

namespace dd
{
	class DebugConsole : public IDebugPanel
	{
	public:

		DebugConsole();
		~DebugConsole();

		virtual const char* GetDebugTitle() const override { return "Console"; }

	protected:
		virtual void DrawDebugInternal() override;

	private:

		char				m_inputBuf[256];
		std::vector<std::string> m_items;
		bool				m_scrollToBottom { false };
		std::vector<std::string> m_history;
		int					m_historyPos { -1 };   // -1: new line, 0..History.Size-1 browsing history.
		std::vector<std::string> m_commands;

		int TextEditCallback(ImGuiInputTextCallbackData* data );
		void AddLog( std::string msg );
		void ExecCommand( std::string command_line );
		void ClearLog();
		void ListFunctions();
		void EvaluateScript( std::string script );
		void ListVariables();
	};
}
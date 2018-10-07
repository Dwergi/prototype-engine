//
// DebugConsole.h - Debug console hooked up with AngelScript and ImGui.
// Copyright (C) Sebastian Nordgren 
// October 12th 2015
//

#pragma once

#include "IDebugPanel.h"

struct ImGuiTextEditCallbackData;

namespace dd
{
	class DebugConsole : public IDebugPanel
	{
	public:

		DebugConsole( AngelScriptEngine& scriptEngine );
		~DebugConsole();

		virtual const char* GetDebugTitle() const override { return "Console"; }

	protected:
		virtual void DrawDebugInternal( ddc::World& world ) override;

	private:

		char				m_inputBuf[256];
		std::vector<std::string> m_items;
		bool				m_scrollToBottom { false };
		std::vector<std::string> m_history;
		int					m_historyPos { -1 };   // -1: new line, 0..History.Size-1 browsing history.
		std::vector<std::string> m_commands;
		AngelScriptEngine&  m_scriptEngine;

		int TextEditCallback( ImGuiTextEditCallbackData* data );
		void AddLog( std::string msg );
		void ExecCommand( std::string command_line );
		void ClearLog();
		void ListFunctions();
		void EvaluateScript( std::string script );
		void ListVariables();
	};
}
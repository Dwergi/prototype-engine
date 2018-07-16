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
		virtual void DrawDebugInternal() override;

	private:

		char				m_inputBuf[256];
		Vector<String128>	m_items;
		bool				m_scrollToBottom;
		Vector<String128>	m_history;
		int					m_historyPos;    // -1: new line, 0..History.Size-1 browsing history.
		Vector<String64>	m_commands;
		AngelScriptEngine&  m_scriptEngine;

		int TextEditCallback( ImGuiTextEditCallbackData* data );
		void AddLog( const char* fmt, ... );
		void ExecCommand( const String& command_line );
		void ClearLog();
		void ListFunctions();
		void EvaluateScript( const String& script );
		void ListVariables();
	};
}
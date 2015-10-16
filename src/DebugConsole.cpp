//
// DebugConsole.cpp - Debug console hooked up with AngelScript and ImGui.
// Copyright (C) Sebastian Nordgren 
// October 12th 2015
//

#include "PrecompiledHeader.h"
#include "DebugConsole.h"

#include "imgui/imgui.h"

// script helpers
namespace
{
	dd::String256 s_scriptOutput;
	dd::WriteStream s_stream( s_scriptOutput );

	void grabInt( int v )
	{
		SerializePOD( dd::Serialize::Mode::JSON, s_stream, v );
	}

	void grabUint( uint v )
	{
		SerializePOD( dd::Serialize::Mode::JSON, s_stream, v );
	}

	void grabBool( bool v )
	{
		SerializePOD( dd::Serialize::Mode::JSON, s_stream, v );
	}

	void grabFloat( float v )
	{
		SerializePOD( dd::Serialize::Mode::JSON, s_stream, v );
	}

	void grabDouble( double v )
	{
		SerializePOD( dd::Serialize::Mode::JSON, s_stream, v );
	}

	void grabString( const dd::String& v )
	{
		SerializeString( dd::Serialize::Mode::JSON, s_stream, v );
	}

	void grab()
	{
		// There is no value
	}
}

namespace dd
{
	void RegisterScriptCommands( Vector<String64>& commands )
	{
		asIScriptEngine* engine = g_services.Get<ScriptEngine>().GetInternalEngine();
		for( uint i = 0; i < engine->GetGlobalFunctionCount(); ++i )
		{
			asIScriptFunction* func = engine->GetGlobalFunctionByIndex( i );

			// Skip the functions that start with _ as these are not meant to be called explicitly by the user
			if( func->GetName()[0] != '_' )
				commands.Add( func->GetName() );
		}

		for( uint i = 0; i < engine->GetGlobalPropertyCount(); ++i )
		{
			const char* name;
			int res = engine->GetGlobalPropertyByIndex( i, &name );
			if( res >= 0 )
				commands.Add( name );
		}
	}

	void RegisterConsoleHelpers()
	{
		ScriptEngine* engine = dd::g_services.GetPtr<ScriptEngine>();
		if( engine == nullptr )
			return;

		// Register special function with overloads to catch any type.
		// This is used by the exec command to output the resulting value from the statement.
		String16 name( "_grab" );

		engine->RegisterGlobalFunction( name, FUNCTION( grabBool ), &grabBool );
		engine->RegisterGlobalFunction( name, FUNCTION( grabInt ), &grabInt );
		engine->RegisterGlobalFunction( name, FUNCTION( grabUint ), &grabUint );
		engine->RegisterGlobalFunction( name, FUNCTION( grabFloat ), &grabFloat );
		engine->RegisterGlobalFunction( name, FUNCTION( grabDouble ), &grabDouble );
		engine->RegisterGlobalFunction( name, FUNCTION( grab ), &grab );
		/*engine->RegisterGlobalFunction( "void _grab(const string &in)", asFUNCTIONPR( grab, (const string&), void ), asCALL_CDECL );*/
	}

	DebugConsole::DebugConsole()
	{
		ClearLog();

		m_inputBuf[0] = 0;
		m_historyPos = -1;

		m_commands.Add( "Help" );
		m_commands.Add( "History" );
		m_commands.Add( "Clear" );
		m_commands.Add( "Functions" );
		m_commands.Add( "Variables" );

		RegisterConsoleHelpers();
		RegisterScriptCommands( m_commands );
	}

	DebugConsole::~DebugConsole()
	{
		ClearLog();

		m_history.Clear();
	}

	void DebugConsole::ClearLog()
	{
		m_items.Clear();

		m_scrollToBottom = true;
	}

	void DebugConsole::AddLog( const char* fmt, ... )
	{
		const int ArraySize = 1024;
		static __declspec(thread) char buf[ArraySize];

		va_list args;
		va_start( args, fmt );
		vsnprintf( buf, ArraySize, fmt, args );
		buf[ArraySize - 1] = 0;
		va_end( args );

		m_items.Add( buf );

		m_scrollToBottom = true;
	}

	void DebugConsole::Draw( const char* title, bool& opened )
	{
		ImGui::SetNextWindowSize( ImVec2( 520, 600 ), ImGuiSetCond_FirstUseEver );
		if( !ImGui::Begin( title, &opened ) )
		{
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );

		static ImGuiTextFilter filter;
		filter.Draw( "Filter (\"incl,-excl\") (\"error\")", 180 );

		ImGui::PopStyleVar();
		ImGui::Separator();

		// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
		// NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
		// If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
		ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -ImGui::GetItemsLineHeightWithSpacing() ), false, ImGuiWindowFlags_HorizontalScrollbar );
		if( ImGui::BeginPopupContextWindow() )
		{
			if( ImGui::Selectable( "Clear" ) )
				ClearLog();
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) ); // Tighten spacing
		for( uint i = 0; i < m_items.Size(); i++ )
		{
			String& item = m_items[i];
			if( !filter.PassFilter( item.c_str() ) )
				continue;

			ImVec4 col = ImColor( 255, 255, 255 ); // A better implementation may store a type per-item. For the sample let's just parse the text.
			if( strstr( item.c_str(), "[error]" ) )
				col = ImColor( 255, 100, 100 );
			else if( strncmp( item.c_str(), "# ", 2 ) == 0 )
				col = ImColor( 255, 200, 150 );

			ImGui::PushStyleColor( ImGuiCol_Text, col );
			ImGui::TextUnformatted( item.c_str() );
			ImGui::PopStyleColor();
		}

		if( m_scrollToBottom )
			ImGui::SetScrollHere();

		m_scrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		auto callbackForwarder = []( ImGuiTextEditCallbackData* pData )->int
		{
			DebugConsole* console = (DebugConsole*) pData->UserData;
			return console->TextEditCallback( pData );
		};

		// Command-line
		if( ImGui::InputText( "Input", m_inputBuf, (int) (sizeof( m_inputBuf ) / sizeof( *m_inputBuf )), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, callbackForwarder, (void*) this ) )
		{
			char* input_end = m_inputBuf + strlen( m_inputBuf );
			while( input_end > m_inputBuf && input_end[-1] == ' ' )
				--input_end;

			*input_end = 0;

			if( m_inputBuf != nullptr )
				ExecCommand( String256( m_inputBuf ) );

			m_inputBuf[0] = 0;
		}

		ImGui::SameLine();

		if( ImGui::SmallButton( "Clear" ) )
			ClearLog();

		// Demonstrate keeping auto focus on the input box
		if( ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked( 0 )) )
			ImGui::SetKeyboardFocusHere( -1 ); // Auto focus

		ImGui::End();
	}

	void DebugConsole::ExecCommand( const String& command_line )
	{
		AddLog( "# %s\n", command_line.c_str() );

		// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
		m_historyPos = -1;
		for( int i = m_history.Size() - 1; i >= 0; --i )
		{
			if( m_history[i].EqualsCaseless( command_line ) )
			{
				m_history.Remove( i );
				break;
			}
		}

		m_history.Add( command_line );

		// Process command
		if( command_line.EqualsCaseless( "Clear" ) )
		{
			ClearLog();
		}
		else if( command_line.EqualsCaseless( "Help" ) )
		{
			AddLog( "Commands:" );
			for( uint i = 0; i < m_commands.Size(); ++i )
				AddLog( "- %s", m_commands[i].c_str() );
		}
		else if( command_line.EqualsCaseless( "History" ) )
		{
			uint history_start = m_history.Size() >= 10 ? m_history.Size() - 10 : 0;

			for( uint i = history_start; i < m_history.Size(); ++i )
				AddLog( "%3d: %s\n", i, m_history[i].c_str() );
		}
		else if( command_line.EqualsCaseless( "Functions" ) )
		{
			ListFunctions();
		}
		else if( command_line.EqualsCaseless( "Variables" ) )
		{
			ListVariables();
		}
		else
		{
			EvaluateScript( command_line );			
		}
	}

	void DebugConsole::EvaluateScript( const String& script )
	{
		ScriptEngine* script_engine = g_services.GetPtr<ScriptEngine>();
		if( script_engine == nullptr )
			return;

		// add our grab commands
		String256 completeString( "_grab(" );
		completeString += script;
		completeString += ")";

		// reset output buffer
		s_scriptOutput.Clear();
		s_stream.Reset();

		String256 errors;

		// pass to AngelScript to evaluate 
		if( script_engine->Evaluate( completeString, errors ) )
			AddLog( "\t%s\n", s_scriptOutput.c_str() );
		else
			AddLog( "\tScript error: %s!", errors );
	}

	void DebugConsole::ListFunctions()
	{
		AddLog( "Functions:\n" );

		asIScriptEngine* engine = g_services.Get<ScriptEngine>().GetInternalEngine();
		for( uint i = 0; i < engine->GetGlobalFunctionCount(); ++i )
		{
			asIScriptFunction* func = engine->GetGlobalFunctionByIndex( i );

			// Skip the functions that start with _ as these are not meant to be called explicitly by the user
			if( func->GetName()[0] != '_' )
				AddLog( "\t- %s\n", func->GetDeclaration() );
		}
	}

	void DebugConsole::ListVariables()
	{
		AddLog( "Variables:\n" );

		asIScriptEngine* engine = g_services.Get<ScriptEngine>().GetInternalEngine();
		for( uint i = 0; i < engine->GetGlobalPropertyCount(); ++i )
		{
			const char* name;
			const char* scope;
			int typeId;
			bool isConst;
			void* ptr;

			int res = engine->GetGlobalPropertyByIndex( i, &name, &scope, &typeId, &isConst, nullptr, &ptr );
			if( res >= 0 )
			{
				const char* type_declaration = engine->GetTypeDeclaration( typeId );

				if( strlen( scope ) > 0 )
				{
					AddLog( "\t- %s %s::%s\n", type_declaration, scope, name );
				}
				else
				{
					AddLog( "\t- %s %s\n", type_declaration, name );
				}
			}
		}
	}

	int DebugConsole::TextEditCallback( ImGuiTextEditCallbackData* data )
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch( data->EventFlag )
		{
			case ImGuiInputTextFlags_CallbackCompletion:
			{
				// Example of TEXT COMPLETION

				String256 buffer( data->Buf );

				uint last_index = data->CursorPos;
				uint first_index = last_index;
				
				while( first_index > 0 )
				{
					char c = buffer[first_index - 1];
					if( isspace( c ) || c == ',' || c == ';' || c == '(' || c == ')' )
						break;

					--first_index;
				}

				uint word_length = last_index - first_index;

				String64 word = buffer.Substring( first_index, word_length );

				// Build a list of candidates
				Vector<String64> candidates;
				for( uint i = 0; i < m_commands.Size(); ++i )
				{
					String64 substring = m_commands[i].Substring( 0, word_length );
					if( substring.EqualsCaseless( word ) )
						candidates.Add( m_commands[i] );
				}

				if( candidates.Size() == 0 )
				{
					// No match
					AddLog( "No match for \"%s\"!\n", buffer.c_str() );
				}
				else if( candidates.Size() == 1 )
				{
					// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
					data->DeleteChars( first_index, word_length );
					data->InsertChars( data->CursorPos, candidates[0].c_str() );
					data->InsertChars( data->CursorPos, " " );
				}
				else
				{
					// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
					uint max_match_length = word_length;
					for( ;; )
					{
						bool all_match = true;
						char test_char;

						for( uint c = 0; c < candidates.Size(); ++c )
						{
							char candidate_char = candidates[c][max_match_length];
							if( c == 0 )
							{
								test_char = candidate_char;
							}
							else if( tolower( test_char ) != tolower( candidate_char ) )
							{
								all_match = false;
								break;
							}
						}

						if( !all_match )
							break;

						++max_match_length;
					}

					if( max_match_length > 0 )
					{
						data->DeleteChars( first_index, last_index - first_index );

						String64 match = candidates[0].Substring( 0, max_match_length );
						data->InsertChars( data->CursorPos, match.c_str() );
					}

					// List matches
					AddLog( "Possible matches:\n" );
					for( uint i = 0; i < candidates.Size(); i++ )
						AddLog( "- %s\n", candidates[i].c_str() );
				}

				break;
			}
			case ImGuiInputTextFlags_CallbackHistory:
			{
				// Example of HISTORY
				const int prev_history_pos = m_historyPos;
				if( data->EventKey == ImGuiKey_UpArrow )
				{
					if( m_historyPos == -1 )
						m_historyPos = m_history.Size() - 1;
					else if( m_historyPos > 0 )
						m_historyPos--;
				}
				else if( data->EventKey == ImGuiKey_DownArrow )
				{
					if( m_historyPos != -1 )
					{
						if( ++m_historyPos >= (int) m_history.Size() )
							m_historyPos = -1;
					}
				}

				// A better implementation would preserve the data on the current input line along with cursor position.
				if( prev_history_pos != m_historyPos )
				{
					snprintf( data->Buf, data->BufSize, "%s", (m_historyPos >= 0) ? m_history[m_historyPos].c_str() : "" );
					data->BufDirty = true;
					data->CursorPos = data->SelectionStart = data->SelectionEnd = (int) strlen( data->Buf );
				}
			}
		}
		return 0;
	}
}
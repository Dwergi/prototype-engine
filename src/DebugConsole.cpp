//
// DebugConsole.cpp - Debug console hooked up with AngelScript and ImGui.
// Copyright (C) Sebastian Nordgren 
// October 12th 2015
//

#include "PCH.h"
#include "DebugConsole.h"

#include "TransformComponent.h"

#include "fmt/format.h"

namespace dd
{
	DebugConsole::DebugConsole()
	{
		ClearLog();

		m_inputBuf[0] = 0;
		m_historyPos = -1;

		m_commands.push_back("Help");
		m_commands.push_back("History");
		m_commands.push_back("Clear");
		m_commands.push_back("Functions");
		m_commands.push_back("Variables");
	}

	DebugConsole::~DebugConsole()
	{
		ClearLog();

		m_history.clear();
	}

	void DebugConsole::ClearLog()
	{
		m_items.clear();

		m_scrollToBottom = true;
	}

	void DebugConsole::AddLog(std::string log)
	{
		m_items.push_back(log);

		m_scrollToBottom = true;
	}

	void DebugConsole::DrawDebugInternal()
	{
		DD_PROFILE_START(DebugConsole_Draw);

		ImGui::SetWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		static ImGuiTextFilter filter;
		filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);

		ImGui::PopStyleVar();
		ImGui::Separator();

		// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
		// NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
		// If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetTextLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear"))
				ClearLog();
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		for (int i = 0; i < m_items.size(); i++)
		{
			std::string& item = m_items[i];
			if (!filter.PassFilter(item.c_str()))
				continue;

			ImVec4 col = ImColor(255, 255, 255); // A better implementation may store a type per-item. For the sample let's just parse the text.
			if (strstr(item.c_str(), "[error]"))
				col = ImColor(255, 100, 100);
			else if (strncmp(item.c_str(), "# ", 2) == 0)
				col = ImColor(255, 200, 150);

			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::TextUnformatted(item.c_str());
			ImGui::PopStyleColor();
		}

		if (m_scrollToBottom)
			ImGui::SetScrollHere();

		m_scrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		auto callbackForwarder = [](ImGuiTextEditCallbackData* pData)->int
		{
			DebugConsole* console = (DebugConsole*) pData->UserData;
			return console->TextEditCallback(pData);
		};

		// Command-line
		if (ImGui::InputText("Input", m_inputBuf, (int) (sizeof(m_inputBuf) / sizeof(*m_inputBuf)), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, callbackForwarder, (void*) this))
		{
			char* input_end = m_inputBuf + strlen(m_inputBuf);
			while (input_end > m_inputBuf && input_end[-1] == ' ')
				--input_end;

			*input_end = 0;

			if (m_inputBuf != nullptr)
				ExecCommand(m_inputBuf);

			m_inputBuf[0] = 0;
		}

		ImGui::SameLine();

		if (ImGui::SmallButton("Clear"))
			ClearLog();

		// Demonstrate keeping auto focus on the input box
		if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
		{
			ImGui::SetKeyboardFocusHere(-1); // Auto focus
		}

		DD_PROFILE_END();
	}

	static bool EqualsCaseless(std::string a, std::string b)
	{
		return _strcmpi(a.c_str(), b.c_str()) == 0;
	}

	void DebugConsole::ExecCommand(std::string command_line)
	{
		AddLog(fmt::format("# {}\n", command_line.c_str()));

		// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
		m_historyPos = -1;
		for (int i = (int) m_history.size() - 1; i >= 0; --i)
		{
			if (EqualsCaseless(m_history[i], command_line))
			{
				m_history.erase(m_history.begin() + i);
				break;
			}
		}

		m_history.push_back(command_line);

		// Process command
		if (EqualsCaseless(command_line, "Clear"))
		{
			ClearLog();
		}
		else if (EqualsCaseless(command_line, "Help"))
		{
			AddLog("Commands:");
			for (int i = 0; i < m_commands.size(); ++i)
			{
				AddLog(fmt::format("{:>3} - {}", i, m_commands[i]));
			}
		}
		else if (EqualsCaseless(command_line, "History"))
		{
			size_t history_start = m_history.size() >= 10 ? m_history.size() - 10 : 0;

			for (size_t i = history_start; i < m_history.size(); ++i)
				AddLog(fmt::format("{:>3} - {}\n", i, m_history[i]));
		}
		else if (EqualsCaseless(command_line, "Functions"))
		{
			ListFunctions();
		}
		else if (EqualsCaseless(command_line, "Variables"))
		{
			ListVariables();
		}
		else
		{
			EvaluateScript(command_line);
		}
	}

	void DebugConsole::EvaluateScript(std::string script)
	{
		// add our grab commands
		std::string completeString = fmt::format("_grab({})", script);

		// reset output buffer
		std::string errors;
	}

	void DebugConsole::ListFunctions()
	{
		AddLog("Functions:\n");
	}

	void DebugConsole::ListVariables()
	{
		AddLog("Variables:\n");
	}

	int DebugConsole::TextEditCallback(ImGuiTextEditCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
			case ImGuiInputTextFlags_CallbackCompletion:
			{
				// Example of TEXT COMPLETION

				std::string buffer(data->Buf);

				int last_index = data->CursorPos;
				int first_index = last_index;

				while (first_index > 0)
				{
					char c = buffer[first_index - 1];
					if (isspace(c) || c == ',' || c == ';' || c == '(' || c == ')')
						break;

					--first_index;
				}

				int word_length = last_index - first_index;

				std::string word = buffer.substr(first_index, word_length);

				// Build a list of candidates
				std::vector<std::string> candidates;
				for (int i = 0; i < m_commands.size(); ++i)
				{
					std::string substring = m_commands[i].substr(0, word_length);
					if (EqualsCaseless(substring, word))
						candidates.push_back(m_commands[i]);
				}

				if (candidates.size() == 0)
				{
					// No match
					AddLog(fmt::format("No match for \"{}\"!\n", buffer));
				}
				else if (candidates.size() == 1)
				{
					// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
					data->DeleteChars(first_index, word_length);
					data->InsertChars(data->CursorPos, candidates[0].c_str());
					data->InsertChars(data->CursorPos, " ");
				}
				else
				{
					// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
					int max_match_length = word_length;
					for (;; )
					{
						bool all_match = true;
						char test_char;

						for (int c = 0; c < candidates.size(); ++c)
						{
							char candidate_char = candidates[c][max_match_length];
							if (c == 0)
							{
								test_char = candidate_char;
							}
							else if (tolower(test_char) != tolower(candidate_char))
							{
								all_match = false;
								break;
							}
						}

						if (!all_match)
							break;

						++max_match_length;
					}

					if (max_match_length > 0)
					{
						data->DeleteChars(first_index, last_index - first_index);

						std::string match = candidates[0].substr(0, max_match_length);
						data->InsertChars(data->CursorPos, match.c_str());
					}

					// List matches
					AddLog("Possible matches:\n");
					for (int i = 0; i < candidates.size(); ++i)
					{
						AddLog(fmt::format("- {}\n", candidates[i].c_str()));
					}
				}

				break;
			}
			case ImGuiInputTextFlags_CallbackHistory:
			{
				// Example of HISTORY
				const int prev_history_pos = m_historyPos;
				if (data->EventKey == ImGuiKey_UpArrow)
				{
					if (m_historyPos == -1)
					{
						m_historyPos = (int) (m_history.size() - 1);
					}
					else if (m_historyPos > 0)
					{
						m_historyPos--;
					}
				}
				else if (data->EventKey == ImGuiKey_DownArrow)
				{
					if (m_historyPos != -1)
					{
						if (++m_historyPos >= (int) m_history.size())
							m_historyPos = -1;
					}
				}

				// A better implementation would preserve the data on the current input line along with cursor position.
				if (prev_history_pos != m_historyPos)
				{
					if (snprintf(data->Buf, data->BufSize, "%s", (m_historyPos >= 0) ? m_history[m_historyPos].c_str() : "") == 0)
					{
						data->BufDirty = true;
						data->CursorPos = data->SelectionStart = data->SelectionEnd = (int) strlen(data->Buf);
					}
				}
			}
		}
		return 0;
	}
}
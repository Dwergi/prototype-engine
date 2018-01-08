//
// IDebugDraw.h - Interface for classes that do debug drawing.
// Copyright (C) Sebastian Nordgren 
// May 2nd 2017
//

#pragma once

namespace dd
{
	class IDebugDraw
	{
	public:

		IDebugDraw() :
			m_debugOpen( false )
		{

		}

		void DrawDebug();

		//
		// Is this debug menu open?
		//
		bool& IsDebugOpen() { return m_debugOpen; }

		virtual const char* GetDebugTitle() const = 0;

	private:

		bool m_debugOpen;

		//
		// Override this to draw your debug UI. You may assume that IsDebugOpen() is true, and that a window already exists. Do NOT call ImGui::End().
		//
		virtual void DrawDebugInternal() = 0;
	};
}
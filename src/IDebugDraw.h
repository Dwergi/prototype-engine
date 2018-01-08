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
		void AddToMenu();

		//
		// Is this debug menu open?
		//
		bool IsDebugOpen() const { return m_debugOpen; }

		//
		// Change the open status of this menu.
		//
		void SetDebugOpen( bool value ) { m_debugOpen = value; }

	private:

		bool m_debugOpen;

		//
		// Override this to draw your debug UI. You may assume that IsDebugOpen() is true, and that a window already exists. Do NOT call ImGui::End().
		//
		virtual void DrawDebugInternal() = 0;
		virtual const char* GetDebugTitle() const = 0;
	};
}
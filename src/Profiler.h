//
// Profiler.h - Macros to use with profiler.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include "IDebugPanel.h"

#define RMT_ENABLED 1
#define RMT_USE_OPENGL 0

#include "Remotery/lib/Remotery.h"

#define DD_PROFILE_INIT() Remotery* profiler; rmt_CreateGlobalInstance( &profiler )

#define DD_PROFILE_THREAD_NAME( Name ) rmt_SetCurrentThreadName( Name )

#define DD_PROFILE_START( Name ) rmt_BeginCPUSample( Name, 0 )

#define DD_PROFILE_END() rmt_EndCPUSample()

#define DD_PROFILE_SCOPED( Name ) rmt_ScopedCPUSample( Name, 0 )

#define DD_PROFILE_DEINIT() rmt_DestroyGlobalInstance( profiler )

#define DD_PROFILE_LOG( LogText ) rmt_LogText( LogText )

#define DD_PROFILE_OGL_START( Name ) rmt_BeginOpenGLSample( Name )

#define DD_PROFILE_OGL_END() rmt_EndOpenGLSample()

#define DD_PROFILE_OGL_SCOPED( Name ) rmt_ScopedOpenGLSample( Name )

#define DD_PROFILE_OGL_INIT() rmt_BindOpenGL()

#define DD_PROFILE_OGL_DEINIT() rmt_UnbindOpenGL()

namespace dd
{
	struct Profiler
	{
		static void MaterialChanged();
		static void ShaderChanged();
		static void RenderStateChanged();
		static void MeshRendered();

		static void BeginFrame();
		static void EndFrame();

		static void Draw();

		static void EnableDraw( bool draw );
		static bool ShouldDraw();
	};

	struct ProfilerValue
	{
		static const int VALUE_COUNT = 100;

		ProfilerValue( const char* name );

		void Increment();
		void SetValue( float value );
		float GetValue( int index ) const;
		int Index() const { return m_index; }

		void BeginFrame();
		void EndFrame();

		void Draw();
		float SlidingAverage() const { return m_sliding; }

	private:
		std::string m_name;
		int m_index { 0 };
		float m_sliding { 0 };
		float m_values[VALUE_COUNT];
	};
}

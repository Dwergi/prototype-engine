//
// Profiler.h - Macros to use with profiler.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include "IDebugPanel.h"
#include "ProfilerValue.h"

#define RMT_ENABLED 1
#define RMT_USE_OPENGL 0

#include "Remotery/lib/Remotery.h"

#define DD_PROFILE_INIT( Remotery ) rmt_CreateGlobalInstance( &Remotery )

#define DD_PROFILE_THREAD_NAME( Name ) rmt_SetCurrentThreadName( Name )

#define DD_PROFILE_START( Name ) rmt_BeginCPUSample( Name, 0 )

#define DD_PROFILE_END() rmt_EndCPUSample()

#define DD_PROFILE_SCOPED( Name ) rmt_ScopedCPUSample( Name, 0 )

#define DD_PROFILE_DEINIT( Remotery ) rmt_DestroyGlobalInstance( Remotery )

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
		static void Initialize();
		static void Shutdown();

		static int FrameCount();

		static void BeginFrame();
		static void EndFrame();

		static void Draw();
		static void EnableDraw( bool draw );
		static bool ShouldDraw() { return s_draw; }

		static ProfilerValue& GetValue(std::string_view full_name);
		
	private:
		static Remotery* s_remotery;

		static std::vector<ProfilerValue*> s_instances;
		static bool s_draw;
		static bool s_inFrame;
		static int s_frameCount;
	};
}

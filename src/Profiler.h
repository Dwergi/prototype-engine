//
// Profiler.h - Macros to use with profiler.
// Copyright (C) Sebastian Nordgren 
// October 29th 2015
//

#pragma once

#include "Remotery/lib/Remotery.h"

#define DD_PROFILE_INIT() \
	Remotery* profiler; rmt_CreateGlobalInstance( &profiler )

#define DD_PROFILE_THREAD_NAME( Name ) \
	rmt_SetCurrentThreadName( Name )

#define DD_PROFILE_START( Name ) \
	rmt_BeginCPUSample( Name, 0 )

#define DD_PROFILE_END() \
	rmt_EndCPUSample()

#define DD_PROFILE_SCOPED( Name ) \
	rmt_ScopedCPUSample( Name, 0 )

#define DD_PROFILE_DEINIT() \
	rmt_DestroyGlobalInstance( profiler )

#define DD_PROFILE_LOG( LogText ) \
	rmt_LogText( LogText )

#define DD_PROFILE_BIND_OPENGL() \
	rmt_BindOpenGL()

#define DD_PROFILE_OGL_START( Name ) \
	rmt_BeginOpenGLSample( Name )

#define DD_PROFILE_OGL_END() \
	rmt_EndOpenGLSample()

#define DD_PROFILE_OGL_SCOPED( Name ) \
	rmt_ScopedOpenGLSample( Name )

#define DD_PROFILE_OGL_INIT() \
	_rmt_BindOpenGL()

#define DD_PROFILE_OGL_DEINIT() \
	rmt_UnbindOpenGL()
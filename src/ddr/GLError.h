//
// GLError.h - Helpers for pinning down OpenGL errors.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#define CheckOGLError() dd::_CheckOGLError(__FILE__, __LINE__, false )
#define DiscardOGLError() dd::_CheckOGLError(__FILE__, __LINE__, true )

namespace dd
{
	void _CheckOGLError( const char* file, int line, bool ignore );
}
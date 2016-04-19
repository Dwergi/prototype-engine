//
// GLError.h - Helpers for pinning down OpenGL errors.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#define CheckGLError() dd::_CheckGLError(__FILE__, __LINE__)

namespace dd
{
	void _CheckGLError( const char* file, int line );
}
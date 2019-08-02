//
// OpenGL.h - Some standard defines for OpenGL.
// Copyright (C) Sebastian Nordgren 
// January 7th 2018
//

#pragma once

#include "GLError.h"

#include "GL/gl3w.h"

typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace OpenGL
{
	static const GLuint InvalidID = 0;

	static bool Initialize();
}

namespace ddr
{
	typedef GLint ShaderLocation;
	static const ShaderLocation InvalidLocation = -1;

	dd::String128 GetArrayUniformName( const char* arrayName, size_t index, const char* uniform = nullptr );
}
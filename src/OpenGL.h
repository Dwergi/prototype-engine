//
// OpenGL.h - Some standard defines for OpenGL.
// Copyright (C) Sebastian Nordgren 
// January 7th 2018
//

#pragma once

typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace OpenGL
{
	static const GLuint InvalidID = 0;
}

namespace dd
{
	typedef GLint ShaderLocation;
	static const ShaderLocation InvalidLocation = -1;

	String128 GetArrayUniformName( const char* arrayName, int index, const char* uniform = nullptr );
}
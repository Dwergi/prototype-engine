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

	bool Initialize();

	void DrawArrays(int verts);
	void DrawArraysInstanced(int verts, int instances);
	void DrawElementsInstanced(int indices, int instances);
}

namespace ddr
{
	typedef GLint ShaderLocation;
	static const ShaderLocation InvalidLocation = -1;

	dd::String64 GetArrayUniformName(const char* arrayName, size_t index, const char* uniform = nullptr);
}
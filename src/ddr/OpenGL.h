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

	enum class Primitive
	{
		Triangles,
		Lines
	};

	bool Initialize();

	void DrawArrays(Primitive primitive, uint64 verts);
	void DrawElements(Primitive primitive, uint64 verts);
	void DrawArraysInstanced(Primitive primitive, uint64 verts, uint64 instances);
	void DrawElementsInstanced(Primitive primitive, uint64 indices, uint64 instances);
}

namespace ddr
{
	typedef GLint ShaderLocation;
	static const ShaderLocation InvalidLocation = -1;

	enum class Normalized
	{
		No,
		Yes
	};

	enum class Instanced
	{
		No,
		Yes
	};

	dd::String64 GetArrayUniformName(const char* arrayName, size_t index, const char* uniform = nullptr);
}

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

namespace ddr
{
	struct VAO;
	struct VBO;

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

	enum class Format
	{
		Float
	};

	enum class Primitive
	{
		Triangles,
		Lines
	};

	dd::String64 GetArrayUniformName(const char* arrayName, size_t index, const char* uniform = nullptr);
}

namespace OpenGL
{
	static const GLuint InvalidID = 0;

	bool Initialize();

	//
	// Draw calls
	//
	void DrawArrays(ddr::Primitive primitive, uint64 verts);
	void DrawElements(ddr::Primitive primitive, uint64 verts);
	void DrawArraysInstanced(ddr::Primitive primitive, uint64 verts, uint64 instances);
	void DrawElementsInstanced(ddr::Primitive primitive, uint64 indices, uint64 instances);

	//
	// VAO functions
	//
	uint CreateVertexArray();
	void DeleteVertexArray(const ddr::VAO& vao);
	void BindVertexArray(const ddr::VAO& vao);
	void UnbindVertexArray();
	void EnableVertexArrayAttribute(const ddr::VAO& vao, ddr::ShaderLocation loc);
	void DisableVertexArrayAttribute(const ddr::VAO& vao, ddr::ShaderLocation loc);
	void SetVertexArrayAttributeFormat(const ddr::VAO& vao, ddr::ShaderLocation loc, ddr::Format format, uint components, ddr::Normalized normalized, uint64 offset);
	void SetVertexArrayBindingDivisor(const ddr::VAO& vao, uint vbo_index, uint divisor);
	void BindVertexArrayAttribute(const ddr::VAO& vao, ddr::ShaderLocation loc, uint vbo_index);
	void SetVertexArrayBuffer(const ddr::VAO& vao, uint index, const ddr::VBO& vbo, uint offset, uint stride);
	void ClearVertexArrayBuffer(const ddr::VAO& vao, uint index);
	void SetVertexArrayIndices(const ddr::VAO& vao, const ddr::VBO& vbo);
	void ClearVertexArrayIndices(const ddr::VAO& vao);

	//
	// VBO functions
	//
	uint CreateBuffer();
	void DeleteBuffer(const ddr::VBO& vbo);
	void BufferStorage(const ddr::VBO& vbo, const dd::IBuffer& buffer);
	void BufferSubData(const ddr::VBO& vbo, const dd::IBuffer& buffer);

	// 
	// TODO: textures, framebuffers, shaders
	//
}

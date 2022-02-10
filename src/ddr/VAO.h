//
// VAO.h - A wrapper around OpenGL VAOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#include "ddr/OpenGL.h"
#include "ddr/VBO.h"

namespace ddr
{
	typedef unsigned int GLuint;
	typedef int GLint;

	struct VBO;

	struct VAO : IVBODestroyedListener
	{
	public:

		VAO();
		VAO(const VAO& other);
		~VAO();

		void Create();
		void Destroy();

		void EnableAttribute(ShaderLocation loc);
		void DisableAttribute(ShaderLocation loc);

		void BindVBO(const VBO& vbo, uint offset, uint stride);
		void UnbindVBO(const VBO& vbo);

		void BindIndices(const VBO& vbo);
		void UnbindIndices(const VBO& vbo);

		void BindAttribute(const VBO& vbo, ShaderLocation loc, GLenum format, Normalized normalized, int components, uint64 offset);

		void Bind();
		void Unbind();

		bool IsValid() const { return m_id != OpenGL::InvalidID; }
		GLuint ID() const { return m_id; }

		static GLint GetCurrentVAO();

	private:

		GLuint m_id { OpenGL::InvalidID };
		GLuint m_indices { OpenGL::InvalidID };
		dd::Array<GLuint, 8> m_vbos;

		bool m_bound { false };

		void OnVBODestroyed(const VBO& vbo) override;
	};
}
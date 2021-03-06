//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#include "OpenGL.h"

namespace ddr
{
	class VBO
	{
	public:

		VBO();
		VBO(const VBO& other);
		~VBO();

		//
		// Create a buffer with a target and usage. 
		// Target is generally GL_ELEMENT_ARRAY_BUFFER for indices, and GL_ARRAY_BUFFER for everything else.
		// Usage is generally GL_STATIC_DRAW.
		//
		void Create(GLenum target, GLenum usage);
		void Destroy();

		template <typename T>
		void SetData(const T* ptr, size_t count) { SetData(dd::ConstBuffer<T>(ptr, count)); }

		void SetData(const dd::IBuffer& buffer);

		const dd::IBuffer& GetData() const { return m_buffer; }

		int GetDataSize() const { return m_buffer.SizeBytes(); }

		void CommitData();

		void Bind();
		void Unbind();

		bool IsValid() const { return m_id != OpenGL::InvalidID; }
		bool IsBound() const;

		GLuint ID() const { return m_id; }

		VBO& operator=(const VBO& other);

		static GLint GetCurrentVBO(GLenum target);

	private:

		GLuint m_id { OpenGL::InvalidID };
		GLenum m_target { OpenGL::InvalidID };
		GLenum m_usage { OpenGL::InvalidID };

		dd::ConstBuffer<byte> m_buffer;
	};
}

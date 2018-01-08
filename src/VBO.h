//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#include "OpenGL.h"

namespace dd
{
	class VBO
	{
	public:

		VBO();
		VBO( const VBO& other );
		~VBO();

		//
		// Create a buffer with a target and usage. 
		// Target is generally GL_ELEMENT_ARRAY_BUFFER for indices, and GL_ARRAY_BUFFER for everything else.
		// Usage is generally GL_STATIC_DRAW.
		//
		void Create( GLenum target, GLenum usage );
		void Destroy();

		void SetData( const IBuffer& buffer );
		void Update();

		void Bind();
		void Unbind();

		bool IsValid() const { return m_id != OpenGL::InvalidID; }
		GLuint ID() const { return m_id; }

		VBO& operator=( const VBO& other );

	private:

		GLuint m_id { OpenGL::InvalidID };
		GLenum m_target { OpenGL::InvalidID };
		GLenum m_usage { OpenGL::InvalidID };

		const IBuffer* m_buffer { nullptr };
	};
}

//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

namespace dd
{
	typedef unsigned int GLuint;
	typedef unsigned int GLenum;

	class VBO
	{
	public:

		VBO();
		VBO( const VBO& other );
		~VBO();

		void Create( GLenum target );
		void Destroy();

		void SetData( const void* data, uint size );
		void Bind();

		bool IsValid() const { return m_id != 0; }
		GLuint ID() const { return m_id; }

		VBO& operator=( const VBO& other );

	private:

		GLuint m_id;
		GLenum m_target;
	};
}

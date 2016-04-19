//
// VAO.h - A wrapper around OpenGL VAOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

namespace dd
{
	typedef unsigned int GLuint;
	typedef int GLint;

	class VAO
	{
	public:

		VAO();
		VAO( const VAO& other );
		~VAO();

		void Create();
		void Destroy();

		void Bind();
		void Unbind();

		bool IsValid() const { return m_id != 0; }
		GLuint ID() const { return m_id; }

		VAO& operator=( const VAO& other );

	private:

		GLuint m_id;
		GLint m_previous;
	};
}
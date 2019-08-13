//
// VAO.h - A wrapper around OpenGL VAOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

namespace ddr
{
	typedef unsigned int GLuint;
	typedef int GLint;

	class VAO
	{
	public:

		VAO();
		VAO(const VAO& other) = delete;
		~VAO();

		void Create();
		void Destroy();

		void Bind();
		void Unbind();

		bool IsValid() const { return m_id != 0; }
		GLuint ID() const { return m_id; }

		static GLint GetCurrentVAO();

	private:

		GLuint m_id { 0 };
		bool m_bound { false };
	};
}
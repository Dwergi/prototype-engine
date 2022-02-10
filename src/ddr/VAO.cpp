//
// VAO.cpp - A wrapper around OpenGL VAOs.
// Copyright (C) Sebastian Nordgren 
// 7th February 2022
//

#include "PCH.h"

#include "ddr/VAO.h"
#include "ddr/VBO.h"

namespace ddr
{
	static dd::Service<ddr::VBOManager> s_vboManager;

	GLint VAO::GetCurrentVAO()
	{
		GLint current = OpenGL::InvalidID;

		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current);
		CheckOGLError();

		return current;
	}

	VAO::VAO()
	{
	}

	VAO::VAO(const VAO& other)
	{
		DD_ASSERT(!m_bound, "Bound VAO shouldn't be copied!");

		m_id = other.m_id;
		m_vbos = other.m_vbos;
	}

	VAO::~VAO()
	{
		DD_ASSERT(!IsValid(), "VAO should be destroyed before destructor!");
	}

	void VAO::Create()
	{
		DD_ASSERT(!IsValid());

		glCreateVertexArrays(1, &m_id);
		CheckOGLError();
	}

	void VAO::Destroy()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(!m_bound);

		glDeleteVertexArrays(1, &m_id);
		CheckOGLError();

		m_id = OpenGL::InvalidID;
	}

	void VAO::EnableAttribute(ShaderLocation loc)
	{
		DD_ASSERT(IsValid());

		glEnableVertexArrayAttrib(m_id, loc);
		CheckOGLError();
	}

	void VAO::DisableAttribute(ShaderLocation loc)
	{ 
		DD_ASSERT(IsValid());

		glDisableVertexArrayAttrib(m_id, loc);
		CheckOGLError();
	}

	void VAO::OnVBODestroyed(const VBO& vbo)
	{
		DD_ASSERT(IsValid());

		bool first = true;

		if (vbo.ID() == m_indices)
		{
			UnbindIndices(vbo);
		}
		else
		{
			UnbindVBO(vbo);
		}
	}

	void VAO::BindVBO(const VBO& vbo, uint offset, uint stride)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_vbos.IndexOf(vbo.ID()) == -1, "Duplicate VBO bound!");
		DD_ASSERT(m_vbos.Size() < m_vbos.Capacity(), "Too many VBOs bound.");

		int index = m_vbos.Size();

		glVertexArrayVertexBuffer(m_id, index, vbo.ID(), offset, stride);
		CheckOGLError();

		m_vbos.Add(vbo.ID());

		s_vboManager->RegisterListenerFor(vbo, this);
	}

	void VAO::UnbindVBO(const VBO& vbo)
	{
		DD_ASSERT(IsValid());

		int index = m_vbos.IndexOf(vbo.ID());
		DD_ASSERT(index != -1, "VBO is not bound.");

		glVertexArrayVertexBuffer(m_id, index, 0, 0, 0);
		CheckOGLError();

		m_vbos.RemoveAt(index);

		s_vboManager->UnregisterListenerFor(vbo, this);
	}

	void VAO::BindIndices(const VBO& vbo)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_indices == OpenGL::InvalidID);

		glVertexArrayElementBuffer(m_id, vbo.ID());
		CheckOGLError();

		m_indices = vbo.ID();

		s_vboManager->RegisterListenerFor(vbo, this);
	}

	void VAO::UnbindIndices(const VBO& vbo)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_indices != OpenGL::InvalidID);

		glVertexArrayElementBuffer(m_id, 0);
		CheckOGLError();

		m_indices = OpenGL::InvalidID;

		s_vboManager->UnregisterListenerFor(vbo, this);
	}

	void VAO::BindAttribute(const VBO& vbo, ShaderLocation loc, GLenum format, Normalized normalized, int components, uint64 offset)
	{
		DD_ASSERT(IsValid());

		glVertexArrayAttribFormat(m_id, loc, components, format, normalized == Normalized::Yes ? GL_TRUE : GL_FALSE, (GLuint) offset);
		CheckOGLError();

		int vbo_index = m_vbos.IndexOf(vbo.ID());
		DD_ASSERT(vbo_index != -1, "VBO not bound!");

		glVertexArrayAttribBinding(m_id, loc, vbo_index);
		CheckOGLError();
	}

	void VAO::Bind()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(GetCurrentVAO() == 0, "Must unbind previous VAO!");

		glBindVertexArray(m_id);
		CheckOGLError();
	}

	void VAO::Unbind()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(GetCurrentVAO() == m_id, "Unbinding different VAO!");

		glBindVertexArray(0);
		CheckOGLError();
	}
}
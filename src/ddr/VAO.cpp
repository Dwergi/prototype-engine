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
		DD_ASSERT(!other.IsValid() || !other.IsBound(), "Bound VAO shouldn't be copied!");

		m_id = other.m_id;
		m_vbos = other.m_vbos;
	}

	VAO::~VAO()
	{
		DD_ASSERT(!IsValid(), "VAO should be Destroy() before destructor!");
	}

	void VAO::Create(std::string_view name)
	{
		DD_ASSERT(!IsValid());

		m_id = OpenGL::CreateVertexArray();
		DD_ASSERT(m_id != OpenGL::InvalidID);

		m_name = name;
		glObjectLabel(GL_VERTEX_ARRAY, m_id, (GLsizei) m_name.length(), m_name.c_str());
	}

	void VAO::Destroy()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(!IsBound(), "Bound VAO shouldn't be destroyed!");

		OpenGL::DeleteVertexArray(*this);

		m_id = OpenGL::InvalidID;
	}

	void VAO::EnableAttribute(ShaderLocation loc)
	{
		DD_ASSERT(IsValid());

		OpenGL::EnableVertexArrayAttribute(*this, loc);
	}

	void VAO::DisableAttribute(ShaderLocation loc)
	{ 
		DD_ASSERT(IsValid());

		OpenGL::DisableVertexArrayAttribute(*this, loc);
	}

	void VAO::OnVBORenamed(const VBO& vbo, uint old_id)
	{
		DD_ASSERT(IsValid());

		bool first = true;

		if (old_id == m_indices)
		{
			OpenGL::SetVertexArrayIndices(*this, vbo);
		}
		else
		{
			// update the ID of the binding
			int index = IndexOf(old_id);
			DD_ASSERT(index != -1, "VBO not bound!");
			BufferBinding& binding = m_vbos[index];
			DD_ASSERT(binding.VBO == &vbo);

			OpenGL::SetVertexArrayBuffer(*this, index, vbo, binding.Offset, binding.Stride);
		}
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

	int VAO::IndexOf(uint vbo_id) const
	{
		for (int i = 0; i < m_vbos.Size(); ++i)
		{
			if (m_vbos[i].VBO->ID() == vbo_id)
			{
				return i;
			}
		}
		return -1;
	}

	void VAO::BindVBO(const VBO& vbo, uint offset, uint stride)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(IndexOf(vbo.ID()) == -1, "VBO already bound!");
		DD_ASSERT(m_vbos.Size() < m_vbos.Capacity(), "Too many VBOs bound.");

		int index = m_vbos.Size();

		BufferBinding binding;
		binding.VBO = &vbo;
		binding.Offset = offset;
		binding.Stride = stride;
		m_vbos.Add(binding);

		OpenGL::SetVertexArrayBuffer(*this, index, vbo, offset, stride);

		s_vboManager->RegisterListenerFor(vbo, this);
	}

	void VAO::UnbindVBO(const VBO& vbo)
	{
		DD_ASSERT(IsValid());

		int index = IndexOf(vbo.ID());
		DD_ASSERT(index != -1, "VBO is not bound.");

		OpenGL::ClearVertexArrayBuffer(*this, index);

		m_vbos.RemoveAt(index);

		s_vboManager->UnregisterListenerFor(vbo, this);
	}

	void VAO::BindIndices(const VBO& vbo)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_indices == OpenGL::InvalidID);

		OpenGL::SetVertexArrayIndices(*this, vbo);

		m_indices = vbo.ID();

		s_vboManager->RegisterListenerFor(vbo, this);
	}

	void VAO::UnbindIndices(const VBO& vbo)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_indices != OpenGL::InvalidID);

		OpenGL::ClearVertexArrayIndices(*this);

		m_indices = OpenGL::InvalidID;

		s_vboManager->UnregisterListenerFor(vbo, this);
	}

	void VAO::CreateAttribute(std::string_view name, ShaderLocation loc, const VBO& vbo, Format format, int components, Normalized normalized, Instanced instanced, uint64 offset)
	{
		DD_ASSERT(IsValid());
		int vbo_index = IndexOf(vbo.ID());
		DD_ASSERT(vbo_index != -1, "VBO not bound!");

		BufferBinding& binding = m_vbos[vbo_index];
		binding.BoundTo.Add(loc);

		Attribute attribute;
		attribute.Name = name;
		attribute.Location = loc;
		attribute.VBO = &vbo;
		attribute.Offset = offset;
		m_attributes.Add(attribute);

		OpenGL::SetVertexArrayAttributeFormat(*this, loc, format, components, normalized, offset);
		OpenGL::BindVertexArrayAttribute(*this, loc, vbo_index);
		OpenGL::SetVertexArrayBindingDivisor(*this, vbo_index, instanced == Instanced::Yes ? 1 : 0);
		OpenGL::EnableVertexArrayAttribute(*this, loc);
	}

	void VAO::Bind()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(GetCurrentVAO() == 0, "Must unbind previous VAO!");

		OpenGL::BindVertexArray(*this);
	}

	void VAO::Unbind()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(IsBound(), "Unbinding different VAO!");

		OpenGL::UnbindVertexArray();
	}

	bool VAO::IsBound() const
	{
		return GetCurrentVAO() == m_id;
	}
}
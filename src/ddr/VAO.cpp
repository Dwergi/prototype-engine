#include "PCH.h"
#include "VAO.h"

#include "OpenGL.h"

namespace ddr
{
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

	VAO::~VAO()
	{
		DD_ASSERT(!m_bound, "Bound VAO shouldn't be destroyed!");
	}

	void VAO::Create()
	{
		DD_ASSERT(!IsValid());

		glGenVertexArrays(1, &m_id);
		CheckOGLError();
	}

	void VAO::Destroy()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(!m_bound);

		glDeleteVertexArrays(1, &m_id);
		CheckOGLError();

		m_id = 0;
	}

	void VAO::Bind()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(!m_bound);
		DD_ASSERT(GetCurrentVAO() == 0, "Must unbind previous VAO!");

		glBindVertexArray(m_id);
		CheckOGLError();

		m_bound = true;
	}

	void VAO::Unbind()
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(m_bound);
		DD_ASSERT(GetCurrentVAO() == m_id, "Unbinding different VAO!");

		glBindVertexArray(0);
		CheckOGLError();

		m_bound = false;
	}
}
#include "PrecompiledHeader.h"
#include "VAO.h"

#include "OpenGL.h"

#include "GL/gl3w.h"

namespace ddr
{
	GLint VAO::GetCurrentVAO()
	{
		GLint current = OpenGL::InvalidID;
		
		glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &current );
		CheckOGLError();

		return current;
	}

	VAO::VAO()
	{
		
	}

	VAO::VAO( const VAO& other ) :
		m_id( other.m_id )
	{
		DD_ASSERT( !other.m_bound, "Can't try to copy a bound VAO!" );
	}

	VAO::~VAO()
	{
		
	}

	VAO& VAO::operator=( const VAO& other )
	{
		DD_ASSERT( !other.m_bound, "Can't try to copy a bound VAO!" );

		m_id = other.m_id;
		m_bound = false;

		return *this;
	}

	void VAO::Create()
	{
		DD_ASSERT( !IsValid() );

		glGenVertexArrays( 1, &m_id );
		CheckOGLError();
	}

	void VAO::Destroy()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( !m_bound );

		glDeleteVertexArrays( 1, &m_id );
		CheckOGLError();

		m_id = 0;
	}

	void VAO::Bind()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( !m_bound );

		GLint current = GetCurrentVAO();
		DD_ASSERT( current == 0, "Must unbind previous VAO!" );

		glBindVertexArray( m_id );
		CheckOGLError();

		m_bound = true;
	}

	void VAO::Unbind()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_bound );

		GLint current = GetCurrentVAO();
		DD_ASSERT( current == m_id, "Unbinding different VAO!" );

		glBindVertexArray( 0 );
		CheckOGLError();

		m_bound = false;
	}
}
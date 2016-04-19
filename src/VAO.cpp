#include "PrecompiledHeader.h"
#include "VAO.h"

#include "GL/gl3w.h"

namespace dd
{
	VAO::VAO() :
		m_id( 0 ),
		m_previous( 0 )
	{
		
	}

	VAO::VAO( const VAO& other ) :
		m_id( other.m_id ),
		m_previous( other.m_previous )
	{
		
	}

	VAO::~VAO()
	{
		
	}

	VAO& VAO::operator=( const VAO& other )
	{
		DD_ASSERT( other.m_previous == 0 );

		m_id = other.m_id;
		m_previous = other.m_previous;

		return *this;
	}

	void VAO::Create()
	{
		DD_ASSERT( !IsValid() );

		glGenVertexArrays( 1, &m_id );
	}

	void VAO::Destroy()
	{
		DD_ASSERT( IsValid() );

		glDeleteVertexArrays( 1, &m_id );
		m_id = 0;
	}

	void VAO::Bind()
	{
		DD_ASSERT( IsValid() );

		GLint prev;
		glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &prev );

		if( prev != m_id )
			m_previous = prev;

		glBindVertexArray( m_id );
	}

	void VAO::Unbind()
	{
		DD_ASSERT( IsValid() );

		glBindVertexArray( m_previous );
		m_previous = 0;
	}
}
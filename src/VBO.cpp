#include "PrecompiledHeader.h"
#include "VBO.h"

#include "GL/gl3w.h"

namespace dd
{
	VBO::VBO() :
		m_id( 0 ),
		m_target( 0 )
	{

	}

	VBO::VBO( const VBO& other ) :
		m_id( other.m_id ),
		m_target( other.m_target )
	{

	}

	VBO::~VBO()
	{

	}

	VBO& VBO::operator=( const VBO& other )
	{
		m_id = other.m_id;
		m_target = other.m_target;

		return *this;
	}

	void VBO::Create( GLenum target )
	{
		DD_ASSERT( !IsValid() );

		glGenBuffers( 1, &m_id );
		m_target = target;
	}

	void VBO::Destroy()
	{
		DD_ASSERT( IsValid() );

		glDeleteBuffers( 1, &m_id );
		m_id = 0;
	}

	void VBO::Bind()
	{
		DD_ASSERT( IsValid() );

		glBindBuffer( m_target, m_id );
	}

	void VBO::SetData( const void* data, uint size )
	{
		glBufferData( m_target, size, data, GL_STATIC_DRAW );
	}
}
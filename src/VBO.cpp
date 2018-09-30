//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PCH.h"
#include "VBO.h"

#include "GLError.h"

namespace ddr
{
	GLint VBO::GetCurrentVBO( GLenum m_target )
	{
		GLint current = OpenGL::InvalidID;

		switch( m_target )
		{
		case GL_ARRAY_BUFFER:
			glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &current );
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &current );
			break;
		default:
			DD_ASSERT( false, "Unknown target specified!" );
			break;
		}

		CheckOGLError();

		return current;
	}

	VBO::VBO()
	{

	}

	VBO::VBO( const VBO& other ) :
		m_id( other.m_id ),
		m_target( other.m_target ),
		m_usage( other.m_usage )
	{

	}

	VBO::~VBO()
	{
		m_id = OpenGL::InvalidID;
		m_target = OpenGL::InvalidID;
		m_usage = OpenGL::InvalidID;
	}

	VBO& VBO::operator=( const VBO& other )
	{
		m_id = other.m_id;
		m_target = other.m_target;
		m_usage = other.m_usage;

		return *this;
	}

	void VBO::Create( GLenum target, GLenum usage )
	{
		DD_ASSERT( !IsValid() );

		DD_ASSERT( target != OpenGL::InvalidID, "Invalid target in VBO::Create!" );
		DD_ASSERT( usage != OpenGL::InvalidID, "Invalid usage in VBO::Create!" );

		glGenBuffers( 1, &m_id );
		CheckOGLError();

		m_target = target;
		m_usage = usage;
	}

	void VBO::Destroy()
	{
		if( IsValid() )
		{
			glDeleteBuffers( 1, &m_id );
			CheckOGLError();
		}

		m_id = OpenGL::InvalidID;
		m_target = OpenGL::InvalidID;
		m_usage = OpenGL::InvalidID;
	}

	bool VBO::IsBound() const
	{
		GLint current = GetCurrentVBO( m_target );
		return current == m_id;
	}

	void VBO::Bind()
	{
		DD_ASSERT( IsValid() );

		GLint current = GetCurrentVBO( m_target );
		DD_ASSERT( current == 0, "VBO already bound to given target, make sure you unbind your buffers!" );

		glBindBuffer( m_target, m_id );
		CheckOGLError();
	}

	void VBO::Unbind()
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( IsBound(), "Unbinding different buffer!" );

		glBindBuffer( m_target, 0 );
		CheckOGLError();
	}

	void VBO::CommitData()
	{
		DD_ASSERT( IsBound() );
		DD_ASSERT( IsValid() );
		DD_ASSERT( m_buffer.IsValid() );

		glNamedBufferData( m_id, m_buffer.SizeBytes(), NULL, m_usage );
		CheckOGLError();

		glNamedBufferData( m_id, m_buffer.SizeBytes(), m_buffer.GetVoid(), m_usage );
		CheckOGLError();
	}

	void VBO::SetData( const dd::IBuffer& buffer )
	{
		if( m_buffer.IsValid() )
		{
			m_buffer.ReleaseConst();
		}

		m_buffer.Set( (byte*) buffer.GetVoid(), buffer.SizeBytes() );
	}
}
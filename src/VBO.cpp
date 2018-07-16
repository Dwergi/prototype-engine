//
// VBO.h - A wrapper around OpenGL VBOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#include "PrecompiledHeader.h"
#include "VBO.h"

#include "GLError.h"

#include "GL/gl3w.h"

namespace ddr
{
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
		CheckGLError();

		m_target = target;
		m_usage = usage;
	}

	void VBO::Destroy()
	{
		if( IsValid() )
		{

			glDeleteBuffers( 1, &m_id );
			CheckGLError();
		}

		m_id = OpenGL::InvalidID;
		m_target = OpenGL::InvalidID;
		m_usage = OpenGL::InvalidID;
	}

	void VBO::Bind()
	{
		DD_ASSERT( IsValid() );

		glBindBuffer( m_target, m_id );
		CheckGLError();
	}

	void VBO::Unbind()
	{
		DD_ASSERT( IsValid() );

		glBindBuffer( m_target, 0 );
		CheckGLError();
	}

	void VBO::Update()
	{
		if( m_buffer.GetVoid() != nullptr )
		{
			glNamedBufferData( m_id, m_buffer.SizeBytes(), NULL, m_usage );
			CheckGLError();

			glNamedBufferData( m_id, m_buffer.SizeBytes(), m_buffer.GetVoid(), m_usage );
			CheckGLError();
		}
	}

	void VBO::SetData( const dd::IBuffer& buffer )
	{
		m_buffer.Set( (byte*) buffer.GetVoid(), buffer.SizeBytes() );

		glNamedBufferData( m_id, buffer.SizeBytes(), buffer.GetVoid(), m_usage );
		CheckGLError();
	}
}
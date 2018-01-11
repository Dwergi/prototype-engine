//
// Texture.cpp - A wrapper around OpenGL textures.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "PrecompiledHeader.h"
#include "Texture.h"

#include "GLError.h"

#include "GL/gl3w.h"

namespace dd
{
	Texture::Texture()
	{

	}

	Texture::~Texture()
	{
		Destroy();
	}

	void Texture::Create( glm::ivec2 size, GLenum format, int mips )
	{
		glGenTextures( 1, &m_id );
		CheckGLError();

		glBindTexture( GL_TEXTURE_2D, m_id );
		CheckGLError();

		m_format = format;
		m_size = size;
		m_mips = mips;

		glTexStorage2D( GL_TEXTURE_2D, mips, m_format, m_size.x, m_size.y );
		CheckGLError();

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		glBindTexture( GL_TEXTURE_2D, 0 );
		
		m_valid = true;
	}

	void Texture::SetData( const ConstBuffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( data.GetVoid() != nullptr );

		int expectedSize = m_size.x * m_size.y * 4 / (1 << mip);
		DD_ASSERT( data.SizeBytes() == expectedSize );
		DD_ASSERT( mip >= 0 );

		glTexImage2D( GL_TEXTURE_2D, mip, m_format, m_size.x, m_size.y, 0, dataFormat, dataType, data.GetVoid() );
		CheckGLError();
	}

	void Texture::GetData( Buffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( data.GetVoid() != nullptr );

		int expectedSize = m_size.x * m_size.y * 4 / (1 << mip);
		DD_ASSERT( data.SizeBytes() == expectedSize );
		DD_ASSERT( mip >= 0 );

		glGetTextureImage( m_id, mip, dataFormat, dataType, data.SizeBytes(), data.Get() );
		CheckGLError();
	}

	void Texture::Destroy()
	{
		if( m_id != OpenGL::InvalidID )
		{
			glDeleteTextures( 1, &m_id );
			CheckGLError();
		}

		m_id = OpenGL::InvalidID;
		m_valid = false;
	}

	void Texture::Bind( int index )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( index >= 0 );

		glActiveTexture( GL_TEXTURE0 + index );
		glBindTexture( GL_TEXTURE_2D, m_id );
		CheckGLError();

		m_textureUnit = index;
	}

	void Texture::Unbind()
	{
		glBindTexture( GL_TEXTURE_2D, 0 );

		m_textureUnit = -1;
	}
}
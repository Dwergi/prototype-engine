//
// Texture.cpp - A wrapper around OpenGL textures.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "PCH.h"
#include "Texture.h"

#include "GLError.h"
#include "OpenGL.h"

namespace ddr
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
		CheckOGLError();

		glBindTexture( GL_TEXTURE_2D, m_id );
		CheckOGLError();

		m_format = format;
		m_size = size;
		m_mips = mips;

		glTexStorage2D( GL_TEXTURE_2D, mips, m_format, m_size.x, m_size.y );
		CheckOGLError();

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

		glBindTexture( GL_TEXTURE_2D, 0 );
		
		m_valid = true;
	}

	void Texture::SetData( const dd::ConstBuffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( data.GetVoid() != nullptr );

		int expectedSize = m_size.x * m_size.y * 4 / (1 << mip);
		DD_ASSERT( data.SizeBytes() == expectedSize );
		DD_ASSERT( mip >= 0 );

		glTextureSubImage2D( m_id, mip, 0, 0, m_size.x, m_size.y, dataFormat, dataType, data.GetVoid() );
		CheckOGLError();
	}

	void Texture::GetData( dd::Buffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( data.GetVoid() != nullptr );

		int expectedSize = m_size.x * m_size.y * 4 / (1 << mip);
		DD_ASSERT( data.SizeBytes() == expectedSize );
		DD_ASSERT( mip >= 0 );

		glGetTextureImage( m_id, mip, dataFormat, dataType, data.SizeBytes(), data.Access() );
		CheckOGLError();
	}

	void Texture::Destroy()
	{
		if( m_id != OpenGL::InvalidID )
		{
			glDeleteTextures( 1, &m_id );
			CheckOGLError();
		}

		m_id = OpenGL::InvalidID;
		m_valid = false;
	}

	void Texture::Bind( int index )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( index >= 0 );

		glBindTextureUnit( index, m_id );
		CheckOGLError();

		m_textureUnit = index;
	}

	void Texture::Unbind()
	{
		glBindTexture( GL_TEXTURE_2D, 0 );

		m_textureUnit = -1;
	}
}
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

	static GLenum GetFormat( int components )
	{
		switch( components )
		{
		case 1:
			return GL_RED;
		case 2:
			return GL_RG;
		case 3:
			return GL_RGB;
		case 4:
			return GL_RGBA;
		}

		DD_ASSERT( false, "Invalid number of components!" );
		return 0;
	}

	void Texture::Create( int width, int height, int components )
	{
		glGenTextures( 1, &m_id );
		CheckGLError();

		glBindTexture( GL_TEXTURE_2D, m_id );
		CheckGLError();

		m_components = components;
		GLenum format = GetFormat( components );

		glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL );
		CheckGLError();

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		CheckGLError();

		m_size = glm::ivec2( width, height );
		m_valid = true;
	}

	void Texture::SetData( const ConstBuffer<byte>& data, int mip )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( data.GetVoid() != nullptr );
		DD_ASSERT( data.SizeBytes() == m_size.x * m_size.y * m_components );
		DD_ASSERT( mip >= 0 );

		GLenum format = GetFormat( m_components );

		glTexImage2D( GL_TEXTURE_2D, mip, format, m_size.x, m_size.y, 0, format, GL_UNSIGNED_BYTE, data.GetVoid() );
		CheckGLError();
	}

	void Texture::GetData( Buffer<byte>& data, int mip )
	{
		DD_ASSERT( IsValid() );
		DD_ASSERT( data.GetVoid() != nullptr );
		DD_ASSERT( data.SizeBytes() == m_size.x * m_size.y * m_components );
		DD_ASSERT( mip >= 0 );

		glGetTextureImage( m_id, mip, GetFormat( m_components ), GL_UNSIGNED_BYTE, data.SizeBytes(), data.Get() );
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
//
// Texture.h - A wrapper around OpenGL textures.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "OpenGL.h"

namespace dd
{
	class Texture
	{
	public:

		Texture();
		~Texture();

		void Create( int width, int height, int components );
		void Destroy();

		glm::ivec2 GetSize() const { return m_size; }

		void SetData( const ConstBuffer<byte>& data, int mip );
		void GetData( Buffer<byte>& data, int mip );

		void Bind( int index );
		void Unbind();

		bool IsValid() const { return m_valid; }
		GLuint ID() const { return m_id; }

		int GetTextureUnit() const { return m_textureUnit; }

	private:

		glm::ivec2 m_size;

		bool m_valid { false };
		GLuint m_id { OpenGL::InvalidID };
		int m_components { 0 };

		int m_textureUnit { -1 };
	};
}
//
// Texture.h - A wrapper around OpenGL textures.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "HandleManager.h"

#include "ddr/OpenGL.h"

namespace ddr
{
	struct Texture : dd::HandleTarget
	{
		//
		// Do not create directly, use TextureManager.
		//
		Texture();
		~Texture();

		void Initialize( glm::ivec2 size, GLenum format, int mips );
		void Create();
		void Destroy();

		glm::ivec2 GetSize() const { return m_size; }

		void SetData( const dd::ConstBuffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType );
		void GetData( dd::Buffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType );

		void Bind( int index );
		void Unbind();

		bool IsValid() const { return m_valid; }
		GLuint ID() const { return m_id; }
		GLenum InternalFormat() const { return m_internalFormat; }

		int GetTextureUnit() const { return m_textureUnit; }

		static const char* TypeName() { return "Texture"; }

	private:

		friend struct TextureManager;

		glm::ivec2 m_size;

		bool m_valid { false };
		int m_mips { 0 };
		GLuint m_id { OpenGL::InvalidID };
		GLenum m_internalFormat { OpenGL::InvalidID };

		int m_textureUnit { -1 };
	};

	using TextureHandle = dd::Handle<ddr::Texture>;

	struct TextureManager : dd::HandleManager<ddr::Texture>
	{
		ddr::TextureHandle Load(std::string_view texture_name);

	private:

		using super = HandleManager<ddr::Texture>;

		virtual void OnCreate( Texture& tex ) const override;
		virtual void OnDestroy( Texture& tex ) const override;
	};
}
//
// Texture.cpp - A wrapper around OpenGL textures.
// Copyright (C) Sebastian Nordgren 
// January 8th 2018
//

#pragma once

#include "PCH.h"
#include "Texture.h"

#include "File.h"
#include "GLError.h"
#include "OpenGL.h"

#include <filesystem>

#include <sfml/Graphics/Image.hpp>

DD_POD_CPP(ddr::TextureHandle);

namespace ddr
{
	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
		Destroy();
	}

	void Texture::Initialize(glm::ivec2 size, GLenum internalFormat, int mips)
	{
		m_internalFormat = internalFormat;
		m_size = size;
		m_mips = mips;
	}

	void Texture::Create()
	{
		if (m_valid)
		{
			// already explicitly created
			return;
		}

		glGenTextures(1, &m_id);
		CheckOGLError();

		glBindTexture(GL_TEXTURE_2D, m_id);
		CheckOGLError();

		glTexStorage2D(GL_TEXTURE_2D, m_mips, m_internalFormat, m_size.x, m_size.y);
		CheckOGLError();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		CheckOGLError();

		glBindTexture(GL_TEXTURE_2D, 0);
		CheckOGLError();

		m_valid = true;
	}

	void Texture::SetData(const dd::ConstBuffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(data.IsValid());

		int expectedSize = m_size.x * m_size.y * 4 / (1 << mip);
		DD_ASSERT(data.SizeBytes() == expectedSize);
		DD_ASSERT(mip >= 0);

		glBindTexture(GL_TEXTURE_2D, m_id);
		CheckOGLError();

		glTextureSubImage2D(m_id, mip, 0, 0, m_size.x, m_size.y, dataFormat, dataType, data.GetVoid());
		CheckOGLError();

		glGenerateMipmap(GL_TEXTURE_2D);
		CheckOGLError();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::GetData(dd::Buffer<byte>& data, int mip, GLenum dataFormat, GLenum dataType)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(data.IsValid());

		int expectedSize = m_size.x * m_size.y * 4 / (1 << mip);
		DD_ASSERT(data.SizeBytes() == expectedSize);
		DD_ASSERT(mip >= 0);

		glGetTextureImage(m_id, mip, dataFormat, dataType, data.SizeBytes(), data.Access());
		CheckOGLError();
	}

	void Texture::Destroy()
	{
		if (!m_valid)
			return;

		glDeleteTextures(1, &m_id);
		CheckOGLError();

		m_id = OpenGL::InvalidID;
		m_valid = false;
	}

	void Texture::Bind(int textureUnit)
	{
		DD_ASSERT(IsValid());
		DD_ASSERT(textureUnit >= 0);

		m_textureUnit = textureUnit;

		glBindTextureUnit(m_textureUnit, m_id);
		CheckOGLError();
	}

	void Texture::Unbind()
	{
		glBindTextureUnit(m_textureUnit, 0);

		m_textureUnit = -1;
	}

	TextureHandle TextureManager::Load(std::string_view name)
	{
		TextureHandle texture_h = super::Find(name); 
		if (!texture_h.IsValid())
		{
			const std::string folder("textures/");
			dd::File file(folder + name);

			sf::Image img;
			if (!img.loadFromFile(file.Path()))
			{
				return TextureHandle();
			}

			const sf::Uint8* data = img.getPixelsPtr();
			glm::ivec2 tex_size(img.getSize().x, img.getSize().y);
			size_t data_size = (size_t) tex_size.x * tex_size.y * 4;
			dd::ConstBuffer<byte> buffer(data, data_size);
			
			texture_h = super::Create(name);
			Texture* texture = texture_h.Access();
			texture->Initialize(tex_size, GL_RGBA8, 1);
			texture->Create();
			texture->SetData(buffer, 0, GL_RGBA, GL_UNSIGNED_BYTE);
		}
		
		return texture_h;
	}

	void TextureManager::OnCreate(Texture& tex) const
	{
		tex.Create();
	}

	void TextureManager::OnDestroy(Texture& tex) const
	{
		tex.Destroy();
	}
}
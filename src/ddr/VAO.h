//
// VAO.h - A wrapper around OpenGL VAOs.
// Copyright (C) Sebastian Nordgren 
// April 19th 2016
//

#pragma once

#include "ddr/OpenGL.h"
#include "ddr/VBO.h"

namespace ddr
{
	typedef unsigned int GLuint;
	typedef int GLint;

	struct VBO;

	struct VAO : IVBOListener
	{
	public:

		VAO();
		VAO(const VAO& other);
		~VAO();

		void Create(std::string_view name);
		void Destroy();

		void EnableAttribute(ShaderLocation loc);
		void DisableAttribute(ShaderLocation loc);

		void BindVBO(const VBO& vbo, uint offset, uint stride);
		void UnbindVBO(const VBO& vbo);

		void BindIndices(const VBO& vbo);
		void UnbindIndices(const VBO& vbo);

		void CreateAttribute(std::string_view name, ShaderLocation loc, const VBO& vbo, Format format, int components, Normalized normalized, Instanced instanced, uint64 offset);

		void Bind();
		void Unbind();
		bool IsBound() const;

		bool IsValid() const { return m_id != OpenGL::InvalidID; }
		GLuint ID() const { return m_id; }

		static GLint GetCurrentVAO();

	private:

		std::string m_name;

		GLuint m_id { OpenGL::InvalidID };
		GLuint m_indices { OpenGL::InvalidID };

		struct BufferBinding
		{
			const VBO* VBO = nullptr;
			uint Offset = 0;
			uint Stride = 0;
			dd::Array<ShaderLocation, 8> BoundTo;
		};
		
		struct Attribute
		{
			std::string Name;
			ShaderLocation Location = ddr::InvalidLocation;
			const VBO* VBO = nullptr;
			uint64 Offset;
		};

		dd::Array<BufferBinding, 8> m_vbos;
		dd::Array<Attribute, 8> m_attributes;

		int IndexOf(uint vbo) const;

		void OnVBORenamed(const VBO& vbo, uint old_id) override;
		void OnVBODestroyed(const VBO& vbo) override;
	};
}
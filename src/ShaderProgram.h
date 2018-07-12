//
// ddr::ShaderProgram.h - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#pragma once

#include "ShaderHandle.h"
#include "OpenGL.h"

#include <unordered_map>

namespace dd
{
	class ICamera;
}

namespace ddr
{
	class Shader;
	class Texture;

	class ShaderProgram
	{
	public:

		//
		// Create a shader with the given name and shaders.
		//
		static ShaderHandle Create( const dd::String& name, const dd::Vector<Shader*>& shaders );

		//
		// Destroy the given shader entirely.
		//
		static void Destroy( ShaderHandle handle );

		//
		// Reload all shaders.
		//
		static void ReloadAll();

		//
		// Get the shader program associated with the given handle.
		// Returns null if none exists or the handle is invalid.
		//
		static ShaderProgram* Get( ShaderHandle handle );

		const dd::String& Name() const { return m_name; }

		bool Reload();

		void Use( bool use );
		bool InUse() const;

		bool IsValid() const { return m_valid; }

		bool BindPositions();
		bool BindNormals();
		bool BindUVs();
		bool BindVertexColours();

		bool BindAttributeFloat( const char* name, uint components, uint stride, uint first, bool normalized );
		void EnableAttribute( const char* name );
		void DisableAttribute( const char* name );

		void SetUniform( const char* name, const glm::mat3& matrix );
		void SetUniform( const char* name, const glm::mat4& matrix );
		void SetUniform( const char* name, const glm::vec3& vector );
		void SetUniform( const char* name, const glm::vec4& vector );
		void SetUniform( const char* name, int i );
		void SetUniform( const char* name, bool b );
		void SetUniform( const char* name, float flt );
		void SetUniform( const char* name, const Texture& texture );

		void SetCamera( const dd::ICamera& camera );

		~ShaderProgram();

		ShaderProgram( const ShaderProgram& other ) = delete;
		ShaderProgram( ShaderProgram&& other ) = delete;
		ShaderProgram& operator=( const ShaderProgram& other ) = delete;
		ShaderProgram& operator=( ShaderProgram&& other ) = delete;

	private:

		friend class ddr::ShaderHandle;

		static std::unordered_map<uint64, ShaderProgram*> s_instances;

		static ShaderProgram* CreateInstance( const dd::String& name, const dd::Vector<Shader*>& shaders );

		dd::String64 m_name;
		bool m_valid { false };
		bool m_inUse { false };
		uint m_id { OpenGL::InvalidID };

		dd::Vector<Shader*> m_shaders;

		explicit ddr::ShaderProgram( const dd::String& name );

		dd::String256 Link();

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;
	};
} 
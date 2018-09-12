//
// ddr::ShaderProgram.h - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#pragma once

#include "ShaderHandle.h"
#include "OpenGL.h"

#include <unordered_map>

namespace ddr
{
	struct Shader;
	class Texture;
	struct UniformStorage;

	struct ShaderProgram
	{
	public:

		// 
		// Load a shader of the given name, assuming that it uses standard extensions. 
		// For anything more advanced, use Create.
		//
		static ShaderHandle Load( const char* name );

		//
		// Create a shader with the given name and shaders.
		//
		static ShaderHandle Create( const char* name, const dd::Vector<Shader*>& shaders );

		//
		// Find a shader with the given name. 
		// Returns an invalid handle if none exists.
		//
		static ShaderHandle Find( const char* name );

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

		void SetUniform( const char* name, const glm::mat3& mat );
		void SetUniform( const char* name, const glm::mat4& mat );
		void SetUniform( const char* name, const glm::vec2& vec );
		void SetUniform( const char* name, const glm::vec3& vec );
		void SetUniform( const char* name, const glm::vec4& vec );
		void SetUniform( const char* name, int i );
		void SetUniform( const char* name, bool b );
		void SetUniform( const char* name, float flt );
		void SetUniform( const char* name, const Texture& texture );

		bool BindPositions();
		bool BindNormals();
		bool BindUVs();
		bool BindVertexColours();

		bool BindAttributeFloat( const char* name, uint components, bool normalized );
		bool BindAttributeVec2( const char* name, bool normalized );
		bool BindAttributeVec3( const char* name, bool normalized );
		bool BindAttributeVec4( const char* name, bool normalized );

		bool SetAttributeInstanced( const char* name );

		bool EnableAttribute( const char* name );
		bool DisableAttribute( const char* name );

		~ShaderProgram();

		ShaderProgram( const ShaderProgram& other ) = delete;
		ShaderProgram( ShaderProgram&& other ) = delete;
		ShaderProgram& operator=( const ShaderProgram& other ) = delete;
		ShaderProgram& operator=( ShaderProgram&& other ) = delete;

	private:

		friend struct ddr::ShaderHandle;
		friend struct ddr::UniformStorage;

		static std::unordered_map<uint64, ShaderProgram*> s_instances;

		static ShaderProgram* CreateInstance( const char* name, const dd::Vector<Shader*>& shaders );

		dd::String64 m_name;
		bool m_valid { false };
		bool m_inUse { false };
		uint m_id { OpenGL::InvalidID };

		dd::Vector<Shader*> m_shaders;

		explicit ddr::ShaderProgram( const char* name );

		dd::String256 Link();

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;
	};
} 
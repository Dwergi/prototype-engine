//
// ddr::ShaderProgram.h - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#pragma once

#include "HandleManager.h"
#include "OpenGL.h"

#include <unordered_map>

namespace ddr
{
	struct Shader;
	struct Texture;
	struct UniformStorage;
	struct ShaderProgram;
	struct ShaderManager;

	struct ScopedShaderUse
	{
		ScopedShaderUse( ShaderProgram& shader );
		ScopedShaderUse( ScopedShaderUse&& other ) : m_shader( other.m_shader ) { other.m_shader = nullptr; }
		~ScopedShaderUse();

	private:
		ShaderProgram* m_shader { nullptr };
	};

	struct ShaderProgram : dd::HandleTarget
	{
		const dd::String& Name() const { return m_name; }

		bool Reload();

		ScopedShaderUse UseScoped();

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

		ShaderProgram();
		~ShaderProgram();

		ShaderProgram( const ShaderProgram& other ) = delete;
		ShaderProgram( ShaderProgram&& other ) = delete;
		ShaderProgram& operator=( const ShaderProgram& other ) = delete;
		ShaderProgram& operator=( ShaderProgram&& other ) = delete;
		
	private:

		friend struct ddr::UniformStorage;
		friend struct ShaderManager;

		bool m_valid { false };
		bool m_inUse { false };
		uint m_id { OpenGL::InvalidID };

		dd::Vector<Shader*> m_shaders;

		void SetShaders( const dd::Vector<Shader*>& shaders );
		dd::String256 Link();

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;

		void AssertBeforeUse( const char* name ) const;
	};

	using ShaderHandle = dd::Handle<ShaderProgram>;

	struct ShaderManager : dd::HandleManager<ShaderProgram>
	{
		// 
		// Load a shader of the given name, assuming that it uses standard extensions. 
		// For anything more advanced, use Create.
		//
		ShaderHandle Load( const char* name );

		//
		// Reload all shaders.
		//
		void ReloadAll();

		static ShaderManager* Instance() { return static_cast<ShaderManager*>( s_singleton ); }

	private:
		using base = HandleManager<ShaderProgram>;
	};
} 
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
	struct ShaderPart;
	struct Texture;
	struct UniformStorage;
	struct Shader;
	struct ShaderManager;

	struct ScopedShader
	{
		ScopedShader( Shader& shader );
		ScopedShader( ScopedShader&& other ) : m_shader( other.m_shader ) { other.m_shader = nullptr; }
		~ScopedShader();

	private:
		Shader* m_shader { nullptr };
	};

	struct Shader : dd::HandleTarget
	{
		const dd::String& Name() const { return m_name; }

		bool Reload();

		ScopedShader UseScoped();

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

		Shader();
		~Shader();

		Shader( const Shader& other ) = delete;
		Shader( Shader&& other ) = delete;
		Shader& operator=( const Shader& other ) = delete;
		Shader& operator=( Shader&& other ) = delete;
		
	private:

		friend struct ddr::UniformStorage;
		friend struct ShaderManager;

		bool m_valid { false };
		bool m_inUse { false };
		uint m_id { OpenGL::InvalidID };

		dd::Vector<ShaderPart*> m_shaders;

		void SetShaders( const dd::Vector<ShaderPart*>& shaders );
		dd::String256 Link();

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;

		void AssertBeforeUse( const char* name ) const;
	};

	using ShaderHandle = dd::Handle<Shader>;

	struct ShaderManager : dd::HandleManager<Shader>
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
		using base = HandleManager<Shader>;
	};
} 
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
		bool Reload();

		ScopedShader UseScoped();

		void Use( bool use );
		bool InUse() const;

		bool IsValid() const { return m_valid; }

		void SetUniform( std::string name, const glm::mat3& mat );
		void SetUniform( std::string name, const glm::mat4& mat );
		void SetUniform( std::string name, const glm::vec2& vec );
		void SetUniform( std::string name, const glm::vec3& vec );
		void SetUniform( std::string name, const glm::vec4& vec );
		void SetUniform( std::string name, int i );
		void SetUniform( std::string name, bool b );
		void SetUniform( std::string name, float flt );
		void SetUniform( std::string name, const Texture& texture );

		bool BindPositions();
		bool BindNormals();
		bool BindUVs();
		bool BindVertexColours();

		bool BindAttributeFloat( std::string name, uint components, bool normalized );
		bool BindAttributeVec2( std::string name, bool normalized );
		bool BindAttributeVec3( std::string name, bool normalized );
		bool BindAttributeVec4( std::string name, bool normalized );

		bool SetAttributeInstanced( std::string name );

		bool EnableAttribute( std::string name );
		bool DisableAttribute( std::string name );

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

		ShaderLocation GetAttribute( std::string name ) const;
		ShaderLocation GetUniform( std::string name ) const;

		void AssertBeforeUse( std::string name ) const;
	};

	using ShaderHandle = dd::Handle<Shader>;

	struct ShaderManager : dd::HandleManager<Shader>
	{
		// 
		// Load a shader of the given name, assuming that it uses standard extensions. 
		// For anything more advanced, use Create.
		//
		ShaderHandle Load( std::string name );

		//
		// Reload all shaders.
		//
		void ReloadAll();

		static ShaderManager* Instance() { return static_cast<ShaderManager*>( s_singleton ); }

	private:
		using base = HandleManager<Shader>;
	};
} 
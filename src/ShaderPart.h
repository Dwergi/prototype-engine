//
// Shader.h - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "OpenGL.h"

namespace ddr
{
	struct UniformStorage;

	struct ShaderPart
	{
	public:

		enum class Type
		{
			Vertex,
			Pixel,
			Geometry
		};

		//
		// Create a shader with the given name and type.
		// Load the contents from the given path.
		//
		static ShaderPart* Create( const std::string& path, Type type );

		//
		// Reload the shader from file.
		//
		bool Reload();

		~ShaderPart();

		ShaderPart( const ShaderPart& other ) = delete;
		ShaderPart( ShaderPart&& other ) = delete;
		ShaderPart& operator=( const ShaderPart& other ) = delete;
		ShaderPart& operator=( ShaderPart&& other ) = delete;
		
	private:

		friend struct Shader;

		Type m_type;
		GLuint m_id { OpenGL::InvalidID };

		std::string m_path;
		std::string m_source;
		
		static std::unordered_map<std::string, ShaderPart*> sm_shaderCache;

		static bool LoadFile( const std::string& path, std::string& outSource );
		static std::string ProcessIncludes( std::string path, std::string source );
		static void GatherUniforms( const std::string& src, UniformStorage& outUniforms );

		ShaderPart( const std::string& path, Type type );

		std::string Compile( const std::string& source );
	};
}
//
// Shader.h - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "OpenGL.h"

namespace ddr
{
	struct Shader
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
		static Shader* Create( const dd::String& path, Type type );

		//
		// Reload the shader from file.
		//
		bool Reload();

		~Shader();

		Shader( const Shader& other ) = delete;
		Shader( Shader&& other ) = delete;
		Shader& operator=( const Shader& other ) = delete;
		Shader& operator=( Shader&& other ) = delete;
		
	private:

		friend struct ShaderProgram;

		Type m_type;
		GLuint m_id;

		dd::String128 m_path;
		dd::String256 m_source;
		
		static std::unordered_map<dd::String128, Shader*> sm_shaderCache;
		static bool LoadFile( const dd::String& path, dd::String& outSource );

		Shader( const dd::String& path, Type type );

		dd::String256 Compile( const dd::String& source );
	};
}
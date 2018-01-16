//
// Shader.h - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include "OpenGL.h"

#include <atomic>

namespace dd
{
	class Shader
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
		static Shader* Create( const String& path, Type type );

		Shader( const Shader& other );
		~Shader();

		Shader& operator=( const Shader& other );

		//
		// Is this shader valid?
		//
		bool IsValid() const { return m_valid; }

		//
		// Reload the shader from file.
		//
		bool Reload();
		
	private:

		friend class ShaderProgram;

		Type m_type;
		GLuint m_id;

		bool m_valid;
		String128 m_path;
		String256 m_source;
		
		std::atomic<int>* m_refCount;

		static std::unordered_map<String128, Shader*> sm_shaderCache;
		static bool LoadFile( const String& path, String& outSource );

		Shader( const String& path, Type type );

		String256 Compile( const String& source );

		void Retain();
		void Release();
	};
}
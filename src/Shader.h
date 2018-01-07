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
		static Shader Create( const String& name, const String& path, Type type );

		Shader( const Shader& other );
		~Shader();

		Shader& operator=( const Shader& other );

		bool IsValid() const { return m_valid; }
		
	private:

		friend class ShaderProgram;

		bool m_valid;
		GLuint m_id;
		String64 m_name;
		std::atomic<int>* m_refCount;

		static DenseMap<String128, String256> sm_shaderCache;
		static bool LoadFile( const String& path, String& outSource );

		Shader( const String& name, Type type );

		String256 Compile( const String& source );

		void Retain();
		void Release();
	};
}
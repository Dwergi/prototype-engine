//
// Shader.h - Wrapper around an OpenGL shader.
// Copyright (C) Sebastian Nordgren 
// April 13th 2016
//

#pragma once

#include <atomic>

namespace dd
{
	namespace OpenGL
	{
		const uint InvalidID = 0;
	}

	class Shader
	{
	public:

		enum class Type
		{
			Vertex,
			Pixel,
			Geometry
		};

		static Shader Create( const String& name, const String& path, Type type );

		Shader( const Shader& other );
		~Shader();

		Shader& operator=( const Shader& other );

		bool IsValid() const { return m_valid; }

	private:

		friend class ShaderProgram;

		bool m_valid;
		uint m_id;
		String64 m_name;
		std::atomic<int>* m_refCount;

		Shader( const String& name, Type type );

		String256 Compile( const String& source );

		void Retain();
		void Release();
	};
}
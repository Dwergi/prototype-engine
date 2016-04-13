//
// ShaderProgram.h - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#pragma once

#include <atomic>

namespace dd
{
	class Shader;

	class ShaderProgram
	{
	public:

		static ShaderProgram Create( const String& name, const Vector<Shader>& shaders );

		ShaderProgram( const ShaderProgram& other );
		~ShaderProgram();

		ShaderProgram& operator=( const ShaderProgram& other );

		void Use( bool use ) const;
		bool InUse() const;

		bool IsValid() const { return m_valid; }

		int GetAttribute( const char* name ) const;
		int GetUniform( const char* name ) const;

	private:

		uint m_id;
		String64 m_name;
		bool m_valid;

		std::atomic<int>* m_refCount;

		explicit ShaderProgram( const String& name );

		String256 Link();

		void Retain();
		void Release();
	};
}
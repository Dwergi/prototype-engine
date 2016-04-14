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

	typedef int GLint;
	typedef GLint ShaderLocation;
	const ShaderLocation InvalidLocation = -1;

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

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;

		bool BindAttributeFloat( const char* name, uint count, uint stride, bool normalized );

		void SetUniform( const char* name, const glm::mat4& matrix ) const;
		void SetUniform( const char* name, const glm::vec3& vec ) const;
		void SetUniform( const char* name, float f ) const;

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
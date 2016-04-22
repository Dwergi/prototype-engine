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
	struct ShaderHandle;

	typedef int GLint;
	typedef GLint ShaderLocation;
	const ShaderLocation InvalidLocation = -1;

	class ShaderProgram
	{
	public:

		static ShaderHandle Create( const String& name, const Vector<Shader>& shaders );
		static ShaderProgram* Get( ShaderHandle handle );
		static void Destroy( ShaderHandle handle );

		ShaderProgram( const ShaderProgram& other );
		~ShaderProgram();

		ShaderProgram& operator=( const ShaderProgram& other );

		const String& Name() const { return m_name; }

		void Use( bool use );
		bool InUse() const;

		bool IsValid() const { return m_valid; }

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;

		bool BindAttributeFloat( const char* name, uint count, uint stride, bool normalized );

		void SetUniform( const char* name, const glm::mat4& matrix ) const;
		void SetUniform( const char* name, const glm::vec3& vec ) const;
		void SetUniform( const char* name, const glm::vec4& vec ) const;
		void SetUniform( const char* name, float f ) const;

	private:

		static std::mutex m_instanceMutex;
		static DenseMap<uint64, ShaderProgram> m_instances;

		static ShaderProgram CreateInstance( const String& name, const Vector<Shader>& shaders );

		uint m_id;
		String64 m_name;
		bool m_valid;
		bool m_inUse;

		std::atomic<int>* m_refCount;

		explicit ShaderProgram( const String& name );

		String256 Link();

		void Retain();
		void Release();
	};

	//
	// A very simple handle to be used to reference a single global instance of a shader in a semi-safe way.
	// Use ShaderProgram::Create to get a handle to a shader.
	//
	struct ShaderHandle
	{
	public:
		ShaderHandle() : m_hash( 0 ) {}
		ShaderProgram* Get() { return ShaderProgram::Get( *this ); }

	private:
		friend class ShaderProgram;
		uint64 m_hash;
	};
}
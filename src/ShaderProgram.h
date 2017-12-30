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
	class ShaderHandle;

	typedef int GLint;
	typedef GLint ShaderLocation;
	const ShaderLocation InvalidLocation = -1;

	class ShaderProgram
	{
	public:

		//
		// Create a shader with the given name and shaders.
		//
		static ShaderHandle Create( const String& name, const Vector<Shader>& shaders );
		static void Destroy( ShaderHandle handle );

		ShaderProgram( const ShaderProgram& other );
		~ShaderProgram();

		ShaderProgram& operator=( const ShaderProgram& other );

		const String& Name() const { return m_name; }

		void Use( bool use );
		bool InUse() const;

		bool IsValid() const { return m_valid; }

		bool BindAttributeFloat( const char* name, uint components, uint stride, uint first, bool normalized );
		void EnableAttribute( const char* name );
		void DisableAttribute( const char* name );

		void SetUniform( const char* name, const glm::mat3& matrix );
		void SetUniform( const char* name, const glm::mat4& matrix );
		void SetUniform( const char* name, const glm::vec3& vector );
		void SetUniform( const char* name, const glm::vec4& vector );
		void SetUniform( const char* name, int i );
		void SetUniform( const char* name, bool b );
		void SetUniform( const char* name, float flt );

	private:

		friend class ShaderHandle;

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

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;

		static ShaderProgram* Get( ShaderHandle handle );
	};

	//
	// A very simple handle to be used to reference a single global instance of a shader in a semi-safe way.
	// Use ShaderProgram::Create to get a handle to a shader.
	//
	class ShaderHandle
	{
	public:
		ShaderHandle() : m_hash( 0 ) {}

		ShaderProgram* Get() { return ShaderProgram::Get( *this ); }

		bool IsValid() const { return m_hash != 0; }

	private:
		friend class ShaderProgram;
		friend class Mesh;

		uint64 m_hash;
	};
} 
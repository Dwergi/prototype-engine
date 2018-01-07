//
// ShaderProgram.h - Wrapper around OpenGL shader program.
// Copyright (C) Sebastian Nordgren 
// February 16th 2016
//

#pragma once

#include "ShaderHandle.h"
#include "OpenGL.h"

#include <atomic>
#include <unordered_map>

namespace dd
{
	class Shader;

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

		void SetPositionsName( const char* name );
		bool BindPositions();

		void SetNormalsName( const char* name );
		bool BindNormals();

		void SetUVsName( const char* name );
		bool BindUVs();

		void SetVertexColoursName( const char* name );
		bool BindVertexColours();

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
		static std::unordered_map<uint64, ShaderProgram> m_instances;

		static ShaderProgram CreateInstance( const String& name, const Vector<Shader>& shaders );

		bool m_valid { false };
		uint m_id { OpenGL::InvalidID };
		String64 m_name;
		bool m_inUse { false };

		String64 m_positionsName;
		String64 m_normalsName;
		String64 m_uvsName;
		String64 m_vertexColoursName;

		std::atomic<int>* m_refCount;

		explicit ShaderProgram( const String& name );

		String256 Link();

		void Retain();
		void Release();

		ShaderLocation GetAttribute( const char* name ) const;
		ShaderLocation GetUniform( const char* name ) const;

		static ShaderProgram* Get( ShaderHandle handle );
	};
} 
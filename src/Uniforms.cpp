//
// Uniforms.cpp - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#include "PrecompiledHeader.h"
#include "Uniforms.h"

#include "ShaderProgram.h"
#include "Texture.h"

namespace ddr
{
	IUniform* UniformStorage::Access( int index )
	{
		return reinterpret_cast<IUniform*>(m_uniforms + index * UNIFORM_SIZE);
	}

	template <typename T>
	void UniformStorage::Create( const char* name, UniformType type, T value )
	{
		DD_ASSERT( strlen( name ) < 256 );

		Uniform<T>* created = (Uniform<T>*) Access( m_count );
		strcpy_s( created->Name, 256, name );
		created->Type = type;
		created->Value = value;

		++m_count;
	}

	UniformStorage::UniformStorage()
	{
	}

	UniformStorage::~UniformStorage()
	{
	}

	void UniformStorage::Set( const char* name, bool value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Boolean, value );
		}
		else
		{
			Create( name, UniformType::Boolean, value );
		}
	}

	void UniformStorage::Set( const char* name, int value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Integer, value );
		}
		else
		{
			Create( name, UniformType::Integer, value );
		}
	}

	void UniformStorage::Set( const char* name, float value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Float, value );
		}
		else
		{
			Create( name, UniformType::Float, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec2 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Vector2, value );
		}
		else
		{
			Create( name, UniformType::Vector2, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec3 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Vector3, value );
		}
		else
		{
			Create( name, UniformType::Vector3, value );
		}
	}

	void UniformStorage::Set( const char* name, glm::vec4 value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Vector4, value );
		}
		else
		{
			Create( name, UniformType::Vector4, value );
		}
	}

	void UniformStorage::Set( const char* name, const glm::mat3& value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Matrix3, value );
		}
		else
		{
			Create( name, UniformType::Matrix3, value );
		}
	}

	void UniformStorage::Set( const char* name, const glm::mat4& value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Matrix4, value );
		}
		else
		{
			Create( name, UniformType::Matrix4, value );
		}
	}

	void UniformStorage::Set( const char* name, const ddr::Texture& value )
	{
		IUniform* uniform = Find( name );
		if( uniform != nullptr )
		{
			SetValue( uniform, UniformType::Texture, value.GetTextureUnit() );
		}
		else
		{
			Create( name, UniformType::Texture, value.GetTextureUnit() );
		}
	}

	void UniformStorage::Bind( ShaderProgram& shader )
	{
		for( int i = 0; i < m_count; ++i )
		{
			IUniform* uniform = Access( i );

			switch( uniform->Type )
			{
				case UniformType::Boolean:
				{
					Uniform<bool>* u = (Uniform<bool>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Integer:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Float:
				{
					Uniform<float>* u = (Uniform<float>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Vector2:
				{
					Uniform<glm::vec2>* u = (Uniform<glm::vec2>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Vector3:
				{
					Uniform<glm::vec3>* u = (Uniform<glm::vec3>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Vector4:
				{
					Uniform<glm::vec4>* u = (Uniform<glm::vec4>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Matrix3:
				{
					Uniform<glm::mat3>* u = (Uniform<glm::mat3>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Matrix4:
				{
					Uniform<glm::mat4>* u = (Uniform<glm::mat4>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
				case UniformType::Texture:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}
			}
		}
	}

	IUniform* UniformStorage::Find( const char* name )
	{
		for( int i = 0; i < m_count; ++i )
		{
			IUniform* uniform = Access( i );

			if( strcmp( uniform->Name, name ) == 0 )
			{
				return uniform;
			}
		}

		return nullptr;
	}
}
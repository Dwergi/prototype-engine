//
// Uniforms.cpp - Container for setting uniforms.
// Copyright (C) Sebastian Nordgren 
// July 13th 2018
//

#include "PCH.h"
#include "Uniforms.h"

#include "Shader.h"
#include "Texture.h"

namespace ddr
{
	UniformType GetUniformTypeFromName( std::string_view type_name )
	{
		if( type_name == "float" )
		{
			return UniformType::Float;
		}
		else if( type_name == "bool" )
		{
			return UniformType::Boolean;
		}
		else if( type_name == "int" )
		{
			return UniformType::Integer;
		}
		else if( type_name == "vec2" )
		{
			return UniformType::Vector2;
		}
		else if( type_name == "vec3" )
		{
			return UniformType::Vector3;
		}
		else if( type_name == "vec4" )
		{
			return UniformType::Vector4;
		}
		else if( type_name == "mat3" )
		{
			return UniformType::Matrix3;
		}
		else if( type_name == "mat4" )
		{
			return UniformType::Matrix4;
		}
		else if( type_name == "sampler2d" )
		{
			return UniformType::Sampler2;
		}

		return UniformType::Invalid;
	}

	template <> UniformType GetUniformType<float>() { return UniformType::Float; }
	template <> UniformType GetUniformType<bool>() { return UniformType::Boolean; }
	template <> UniformType GetUniformType<int>() { return UniformType::Integer; }
	template <> UniformType GetUniformType<glm::vec2>() { return UniformType::Vector2; }
	template <> UniformType GetUniformType<glm::vec3>() { return UniformType::Vector3; }
	template <> UniformType GetUniformType<glm::vec4>() { return UniformType::Vector4; }
	template <> UniformType GetUniformType<glm::mat3>() { return UniformType::Matrix3; }
	template <> UniformType GetUniformType<glm::mat4>() { return UniformType::Matrix4; }
	template <> UniformType GetUniformType<Texture>() { return UniformType::Sampler2; }

	IUniform* UniformStorage::Access( int index )
	{
		return reinterpret_cast<IUniform*>(m_uniforms + index * UNIFORM_SIZE);
	}

	void UniformStorage::Create( const char* name, UniformType type )
	{
		DD_ASSERT( strlen( name ) < MAX_UNIFORM_NAME );

		IUniform* created = Access( m_count );
		strcpy_s( created->Name, MAX_UNIFORM_NAME, name );
		created->Type = type;

		++m_count;
	}

	UniformStorage::UniformStorage()
	{
	}

	UniformStorage::~UniformStorage()
	{
	}

	template <typename T>
	void UniformStorage::SetValue( IUniform* uniform, const T& value )
	{
		DD_ASSERT( uniform->Type == GetUniformTypeFor( value ) );

		((Uniform<T>*) uniform)->Value = value;
	}

	template <typename T>
	void UniformStorage::SetHelper( const char* name, const T& value )
	{
		UniformType type = GetUniformType<T>();

		IUniform* uniform = Find( name );
		if( uniform == nullptr )
		{
			Create( name, type );
		}

		DD_ASSERT( uniform->Type == type );
		SetValue( uniform, value );

		if( m_shader != nullptr )
		{
			m_shader->SetUniform( name, value );
		}
	}

	void UniformStorage::Set( const char* name, bool value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, int value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, float value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, glm::vec2 value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, glm::vec3 value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, glm::vec4 value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, const glm::mat3& value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, const glm::mat4& value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( const char* name, const ddr::Texture& value )
	{
		IUniform* uniform = Find( name );
		if( uniform == nullptr )
		{
			IUniform* created = Access( m_count );
			strcpy_s( created->Name, MAX_UNIFORM_NAME, name );
			created->Type = UniformType::Sampler2;
		}

		((Uniform<int>*) uniform)->Value = value.GetTextureUnit();

		if( m_shader != nullptr )
		{
			m_shader->SetUniform( name, value );
		}
	}

	void UniformStorage::Bind( Shader& shader )
	{
		DD_ASSERT( m_shader == nullptr, "UniformStorage already bound!" );
		DD_ASSERT( m_shader->InUse(), "Shader not in use!" );

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
				case UniformType::Sampler2:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( u->Name, u->Value );
					break;
				}

				default:
					DD_ASSERT( false, "Invalid uniform type!" );
			}
		}

		m_shader = &shader;
	}

	void UniformStorage::Unbind()
	{
		DD_ASSERT( m_shader != nullptr, "UniformStorage not bound!" );

		m_shader = nullptr;
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
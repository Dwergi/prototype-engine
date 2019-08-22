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
		else if( type_name == "sampler2D" )
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

	UniformStorage::UniformStorage()
	{
	}

	UniformStorage::~UniformStorage()
	{
	}

	IUniform* UniformStorage::Access( int index )
	{
		return reinterpret_cast<IUniform*>(m_storage + index * UNIFORM_SIZE);
	}

	IUniform* UniformStorage::Create( std::string_view name, UniformType type )
	{
		int index = (int) m_uniforms.size();

		IUniform* created = Access( index );
		created->Type = type;

		m_uniforms.insert( std::make_pair( name, index ) );

		return created;
	}

	template <typename T>
	void UniformStorage::SetValue( IUniform* uniform, const T& value )
	{
		DD_ASSERT( uniform->Type == GetUniformTypeFor( value ) );

		((Uniform<T>*) uniform)->Value = value;
	}

	template <typename T>
	void UniformStorage::SetHelper( std::string_view name, const T& value )
	{
		UniformType type = GetUniformType<T>();

		IUniform* uniform = Find( name );
		if( uniform == nullptr )
		{
			uniform = Create( name, type );
		}

		DD_ASSERT( uniform->Type == type );
		SetValue( uniform, value );

		if( m_shader != nullptr )
		{
			m_shader->SetUniform( name, value );
		}
	}

	void UniformStorage::Set( std::string_view name, bool value )
	{
		DD_TODO("I think these strings are copying...");
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, int value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, float value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, glm::vec2 value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, glm::vec3 value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, glm::vec4 value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, const glm::mat3& value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, const glm::mat4& value )
	{
		SetHelper( name, value );
	}

	void UniformStorage::Set( std::string_view name, const ddr::Texture& value )
	{
		IUniform* uniform = Find( name );
		if( uniform == nullptr )
		{
			Create( name, UniformType::Sampler2 );
			uniform = Find( name );
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
		DD_ASSERT( shader.InUse(), "Shader not in use!" );

		for( auto pair : m_uniforms )
		{
			std::string_view name = pair.first;
			IUniform* uniform = Access( pair.second );

			switch( uniform->Type )
			{
				case UniformType::Boolean:
				{
					Uniform<bool>* u = (Uniform<bool>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Integer:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Float:
				{
					Uniform<float>* u = (Uniform<float>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Vector2:
				{
					Uniform<glm::vec2>* u = (Uniform<glm::vec2>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Vector3:
				{
					Uniform<glm::vec3>* u = (Uniform<glm::vec3>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Vector4:
				{
					Uniform<glm::vec4>* u = (Uniform<glm::vec4>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Matrix3:
				{
					Uniform<glm::mat3>* u = (Uniform<glm::mat3>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Matrix4:
				{
					Uniform<glm::mat4>* u = (Uniform<glm::mat4>*) uniform;
					shader.SetUniform( name, u->Value );
					break;
				}
				case UniformType::Sampler2:
				{
					Uniform<int>* u = (Uniform<int>*) uniform;
					shader.SetUniform( name, u->Value );
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

	void UniformStorage::CopyValue( IUniform* dst, const IUniform* src )
	{
		DD_ASSERT( dst->Type == src->Type );

		switch( src->Type )
		{
		case UniformType::Boolean:
		{
			Uniform<bool>* u = (Uniform<bool>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Integer:
		{
			Uniform<int>* u = (Uniform<int>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Float:
		{
			Uniform<float>* u = (Uniform<float>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Vector2:
		{
			Uniform<glm::vec2>* u = (Uniform<glm::vec2>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Vector3:
		{
			Uniform<glm::vec3>* u = (Uniform<glm::vec3>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Vector4:
		{
			Uniform<glm::vec4>* u = (Uniform<glm::vec4>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Matrix3:
		{
			Uniform<glm::mat3>* u = (Uniform<glm::mat3>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Matrix4:
		{
			Uniform<glm::mat4>* u = (Uniform<glm::mat4>*) src;
			SetValue( dst, u->Value );
			break;
		}
		case UniformType::Sampler2:
		{
			Uniform<int>* u = (Uniform<int>*) src;
			SetValue( dst, u->Value );
			break;
		}
		default:
			DD_ASSERT( false, "Invalid uniform type!" );
		}
	}

	void UniformStorage::GetValuesFrom( UniformStorage& other )
	{
		for( auto pair : m_uniforms )
		{
			IUniform* other_uniform = other.Find( pair.first );
			if( other_uniform != nullptr )
			{
				IUniform* this_uniform = Access( pair.second );
				CopyValue( this_uniform, other_uniform );
			}
		}
	}

	IUniform* UniformStorage::Find( std::string_view name ) 
	{
		auto it = m_uniforms.find( name.data() );
		if (it == m_uniforms.end())
		{
			return nullptr;
		}

		return Access( it->second );
	}

	void UniformStorage::Clear()
	{
		m_uniforms.clear();
	}
}
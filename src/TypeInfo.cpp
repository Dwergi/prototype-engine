//
// TypeInfo.cpp - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Significantly influenced by Randy Gaul (http://RandyGaul.net)
//

#include "PrecompiledHeader.h"
#include "TypeInfo.h"

namespace dd
{
	std::unordered_map<SharedString, TypeInfo*> TypeInfo::sm_typeMap;

	bool TypeInfo::sm_defaultsRegistered = false;

	AngelScriptEngine* TypeInfo::sm_scriptEngine = nullptr;

	void FindNamespace( const char* name, String& nameSpace, String& typeName )
	{
		String128 input( name );
		uint offset = 0;
		while( true )
		{
			int found = input.Find( "::", offset );
			if( found < 0 )
			{
				break;
			}

			offset = found + 2;
		}

		if( offset > 0 )
		{
			nameSpace = input.Substring( 0, offset - 2 );
			typeName = input.Substring( offset );
		}
		else
		{
			nameSpace = "";
			typeName = input;
		}
	}

	TypeInfo::TypeInfo()
	{
	}

	void TypeInfo::Init( const char* name, uint size )
	{
		String128 typeName, nameSpace;
		FindNamespace( name, nameSpace, typeName );

		m_name = typeName;
		m_namespace = nameSpace;
		m_size = size;
	}

	const Member* TypeInfo::GetMember( const char* memberName ) const
	{
		for( const Member& member : m_members )
		{
			if( member.m_name == memberName )
				return &member;
		}

		return nullptr;
	}

	const Function* TypeInfo::GetMethod( const char* methodName ) const
	{
		for( const Method& m : m_methods )
		{
			if( m.Name == methodName )
				return &m.Function;
		}

		return nullptr;
	}

	const TypeInfo* TypeInfo::GetType( const char* typeName )
	{
		return GetType( SharedString( typeName ) );
	}

	const TypeInfo* TypeInfo::GetType( const String& typeName )
	{
		return GetType( SharedString( typeName ) );
	}

	const TypeInfo* TypeInfo::GetType( const SharedString& typeName )
	{
		DD_ASSERT( sm_typeMap.find( typeName ) != sm_typeMap.end() );

		auto it = sm_typeMap.find( typeName );
		if( it == sm_typeMap.end() )
			return nullptr;

		return it->second;
	}

	bool TypeInfo::IsDerivedFrom( const TypeInfo* type ) const
	{
		const TypeInfo* current = this;

		while( current != nullptr )
		{
			if( current == type )
				return true;

			current = current->m_parentType;
		}
		
		return false;
	}

	String128 TypeInfo::FullTypeName() const
	{
		String128 fullName;
		fullName += m_namespace.c_str();
		fullName += "::";
		fullName += m_name.c_str();

		return fullName;
	}

	void TypeInfo::RegisterDefaultTypes()
	{
		sm_defaultsRegistered = true;

		// integers
		DD_REGISTER_POD( int );
		DD_REGISTER_POD( char );
		DD_REGISTER_POD( int16 );
		DD_REGISTER_POD( int64 );
		
		// unsigned integers
		DD_REGISTER_POD( uint );
		DD_REGISTER_POD( byte );
		DD_REGISTER_POD( uint16 );
		DD_REGISTER_POD( uint64 );

		DD_REGISTER_POD( float );
		DD_REGISTER_POD( double );
		
		DD_REGISTER_POD( char* );

		DD_REGISTER_POD( bool );

		DD_REGISTER_TYPE( dd::String );

		DD_REGISTER_TYPE( dd::String8 );
		DD_REGISTER_PARENT( dd::String8, dd::String );

		DD_REGISTER_TYPE( dd::String16 );
		DD_REGISTER_PARENT( dd::String16, dd::String );

		DD_REGISTER_TYPE( dd::String32 );
		DD_REGISTER_PARENT( dd::String32, dd::String );
		
		DD_REGISTER_TYPE( dd::String64 );
		DD_REGISTER_PARENT( dd::String64, dd::String );
		
		DD_REGISTER_TYPE( dd::String128 );
		DD_REGISTER_PARENT( dd::String128, dd::String );

		DD_REGISTER_TYPE( dd::String256 );
		DD_REGISTER_PARENT( dd::String256, dd::String );

		DD_REGISTER_TYPE( dd::SharedString );
	}

	void TypeInfo::SetScriptEngine( AngelScriptEngine* scriptEngine )
	{
		sm_scriptEngine = scriptEngine;
	}
}
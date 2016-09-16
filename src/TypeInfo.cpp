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
	DenseMap<SharedString,TypeInfo*> TypeInfo::sm_typeMap;

	bool TypeInfo::sm_defaultsRegistered = false;

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
		: m_size( 0 )
	{
	}

	void TypeInfo::Init( const char* name, uint size )
	{
		String128 typeName, nameSpace;
		FindNamespace( name, nameSpace, typeName );

		m_name = typeName;
		m_namespace = nameSpace;
		m_size = size;
		m_containedType = nullptr;
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
		DD_ASSERT( sm_typeMap.Contains( typeName ) );

		TypeInfo** pType = sm_typeMap.Find( typeName );

		if( pType == nullptr )
			return nullptr;

		return *pType;
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
		REGISTER_POD( int );
		REGISTER_POD( char );
		REGISTER_POD( int16 );
		REGISTER_POD( int64 );
		
		// unsigned integers
		REGISTER_POD( uint );
		REGISTER_POD( byte );
		REGISTER_POD( uint16 );
		REGISTER_POD( uint64 );

		REGISTER_POD( float );
		REGISTER_POD( double );
		
		REGISTER_POD( char* );

		REGISTER_POD( bool );

		REGISTER_TYPE( dd::String );

		REGISTER_TYPE( dd::String8 );
		REGISTER_PARENT( dd::String8, dd::String );

		REGISTER_TYPE( dd::String16 );
		REGISTER_PARENT( dd::String16, dd::String );

		REGISTER_TYPE( dd::String32 );
		REGISTER_PARENT( dd::String32, dd::String );
		
		REGISTER_TYPE( dd::String64 );
		REGISTER_PARENT( dd::String64, dd::String );
		
		REGISTER_TYPE( dd::String128 );
		REGISTER_PARENT( dd::String128, dd::String );

		REGISTER_TYPE( dd::String256 );
		REGISTER_PARENT( dd::String256, dd::String );
	}
}
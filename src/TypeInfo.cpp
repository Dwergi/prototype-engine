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
	DenseMap<String32,TypeInfo*> TypeInfo::sm_typeMap;

	TypeInfo::TypeInfo()
		: m_size( 0 )
	{
	}

	void TypeInfo::Init( const char* name, unsigned size )
	{
		m_name = name;
		m_size = size;
		m_containedType = nullptr;
	}

	void TypeInfo::AddMember( const TypeInfo* typeInfo, const char* name, uint offset )
	{
		Member& member = m_members.Allocate();
		member.m_name = name;
		member.m_offset = offset;
		member.m_typeInfo = typeInfo;
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
		return GetType( String32( typeName ) );
	}

	const TypeInfo* TypeInfo::GetType( const String& typeName )
	{
		ASSERT( sm_typeMap.Contains( typeName ) );

		TypeInfo** pType = sm_typeMap.Find( typeName );

		if( pType == nullptr )
			return nullptr;

		return *pType;
	}

	bool TypeInfo::HasCustomSerializers() const
	{
		return SerializeCustom != nullptr && DeserializeCustom != nullptr;
	}

	void TypeInfo::SetCustomSerializers( SerializeFn serializer, DeserializeFn deserializer )
	{
		SerializeCustom = serializer;
		DeserializeCustom = deserializer;
	}

	void RegisterDefaultTypes()
	{
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

		REGISTER_TYPE( String );
		SET_SERIALIZERS( String, Serialize::SerializeString, Serialize::DeserializeString );
		REGISTER_TYPE( String8 );
		SET_SERIALIZERS( String8, Serialize::SerializeString, Serialize::DeserializeString );
		REGISTER_TYPE( String16 );
		SET_SERIALIZERS( String16, Serialize::SerializeString, Serialize::DeserializeString );
		REGISTER_TYPE( String32 );
		SET_SERIALIZERS( String32, Serialize::SerializeString, Serialize::DeserializeString );
		REGISTER_TYPE( String64 );
		SET_SERIALIZERS( String64, Serialize::SerializeString, Serialize::DeserializeString );
		REGISTER_TYPE( String128 );
		SET_SERIALIZERS( String128, Serialize::SerializeString, Serialize::DeserializeString );
		REGISTER_TYPE( String256 );
		SET_SERIALIZERS( String256, Serialize::SerializeString, Serialize::DeserializeString );
	}
}
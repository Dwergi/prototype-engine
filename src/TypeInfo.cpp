//
// TypeInfo.cpp - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Significantly influenced by Randy Gaul (http://RandyGaul.net)
//

#include "PrecompiledHeader.h"
#include "TypeInfo.h"

#include "Serializers.h"

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
	}

	void TypeInfo::AddMember( const TypeInfo* typeInfo, const char* name, unsigned offset )
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

	const TypeInfo* TypeInfo::GetType( const String32& typeName )
	{
		ASSERT( sm_typeMap.Contains( typeName ) );

		TypeInfo** pType = sm_typeMap.Find( typeName );

		if( pType == nullptr )
			return nullptr;

		return *pType;
	}

	String&& TypeInfo::SerializePOD( SerializationMode mode, const void* data ) const
	{
		return std::move( String256( dd::SerializePOD( mode, data, m_format.c_str(), m_size ) ) );
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
		REGISTER_POD( int, "%d" );
		REGISTER_POD( int8, "%hhd" );
		REGISTER_POD( int16, "%hd" );
		REGISTER_POD( int32, "%d" );
		REGISTER_POD( int64, "%lld" );
		REGISTER_POD( char, "%c" );

		// unsigned integers
		REGISTER_POD( uint, "%u" );
		REGISTER_POD( uint8, "%hhu" );
		REGISTER_POD( uint16, "%hu" );
		REGISTER_POD( uint32, "%u" );
		REGISTER_POD( uint64, "%llu" );
		REGISTER_POD( byte, "%hhu" );

		REGISTER_POD( float, "%f" );
		REGISTER_POD( double, "%lf" );
		
		REGISTER_POD( char*, "%s" );

		REGISTER_TYPE( String );

		SET_SERIALIZERS( String, SerializeString, DeserializeString );
	}
}
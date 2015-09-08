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

/*
	void TypeInfo::SetSerializer( SerializeCB cb )
	{
		m_serialize = cb;
	}

	void TypeInfo::SetDeserializer( DeserializeCB cb )
	{
		m_deserialize = cb;
	}

	void TypeInfo::Serialize( File& file, Variable var ) const
	{
		if(m_serialize)
			m_serialize( file, var );
		else
			TextSerializer::Get()->Serialize( file, var );
	}

	void TypeInfo::Deserialize( File& file, Variable var ) const
	{
		if(m_deserialize)
			m_deserialize( file, var );
		else
			TextSerializer::Get()->Deserialize( file, var );
	}

	void TypeInfo::SetToLua( ToLuaCB cb )
	{
	m_toLua = cb;
	}

	void TypeInfo::SetFromLua( FromLuaCB cb )
	{
	m_fromLua = cb;
	}

	void TypeInfo::ToLua( lua_State* L, Variable var ) const
	{
	if(m_toLua)
	m_toLua( L, var );
	else
	Lua::GenericToLua( L, var );
	}

	void TypeInfo::FromLua( lua_State* L, int index, Variable* var ) const
	{
	if(m_fromLua)
	m_fromLua( L, index, var );
	else
	Lua::GenericFromLua( L, index, var );
	}

	const char* TypeInfo::LuaMetaTable() const
	{
	return m_metatable;
	}*/

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

	void RegisterDefaultTypes()
	{
		// integers
		REGISTER_POD( int );
		REGISTER_POD( int8 );
		REGISTER_POD( int16 );
		REGISTER_POD( int32 );
		REGISTER_POD( int64 );
		REGISTER_POD( char );

		// unsigned integers
		REGISTER_POD( uint );
		REGISTER_POD( uint8 );
		REGISTER_POD( uint16 );
		REGISTER_POD( uint32 );
		REGISTER_POD( uint64 );
		REGISTER_POD( unsigned int );
		REGISTER_POD( byte );

		REGISTER_POD( float );
		REGISTER_POD( double );
		
		REGISTER_POD( const char* );
	}
}
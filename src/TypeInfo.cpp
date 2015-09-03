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
	DenseMap<String,TypeInfo*> TypeInfo::sm_typeMap;

	const TypeInfo* Member::Type() const
	{
		return m_typeInfo;
	}

	unsigned Member::Offset() const
	{
		return m_offset;
	}

	const dd::String& Member::Name() const
	{
		return m_name;
	}

	Property Member::GetProperty() const
	{
		return m_property;
	}

	TypeInfo::TypeInfo()/*
		: m_serialize( NULL )
		, m_deserialize( NULL )
		, m_toLua( NULL )
		, m_fromLua( NULL )
		, m_metatable( NULL )*/
	{
	}

	void TypeInfo::Init( const char* name, unsigned size )
	{
		m_name = name;
		m_size = size;
	}

	void TypeInfo::AddMember( const TypeInfo* typeInfo, const char* name, unsigned offset )
	{
		Member mem;
		mem.m_name = name;
		mem.m_offset = offset;
		mem.m_typeInfo = typeInfo;
		m_members.Push( mem );
	}

	const Member* TypeInfo::GetMember( const char* memberName ) const
	{
		for( uint i = 0; i < m_members.Size(); ++i )
		{
			if( m_members[i].m_name == memberName )
				return &m_members[i];
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

	void TypeInfo::AddProperty( const char* memberName, const Property& prop )
	{
		Member* m = (Member*) GetMember( memberName );
		m->m_property = prop;
	}

	unsigned TypeInfo::Size() const
	{
		return m_size;
	}

	const char* TypeInfo::Name() const
	{
		return m_name.c_str();
	}

	const TypeInfo* TypeInfo::GetType( const char* typeName )
	{
		return GetType( String( typeName ) );
	}

	const TypeInfo* TypeInfo::GetType( const String& typeName )
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
		
		REGISTER_POD( const char*  );
		REGISTER_TYPE( String );
	}
}
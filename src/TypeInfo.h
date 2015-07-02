#pragma once

#include "AutoList.h"

class MemberBase;

template< typename T >
class HasTypeInfo
{
public:
	HasTypeInfo()
	{
		T::RegisterType();
	}
};

class TypeInfo : public AutoList<TypeInfo>
{
public:

	TypeInfo();
	~TypeInfo();

	template< typename T >
	static TypeInfo* GetType()
	{
		static TypeInfo s_typeInfo;
		return &s_typeInfo;
	}

	static TypeInfo* GetType( const std::string& name );

	template< typename T >
	static void Register( const char* name )
	{
		TypeInfo* type = GetType<T>();
		type->Init<T>( name, sizeof( T ) );
	}

	const std::vector<MemberBase*>& GetMembers()
	{
		return m_members;
	}

private:

	size_t m_size;
	std::string m_name;
	std::vector<MemberBase*> m_members;

	template< typename T >
	void Init( const char* name, size_t size )
	{
		if( m_size > 0 )
			return;

		m_size = size;
		m_name = name;

		T::RegisterMembers( m_members );
	}
};
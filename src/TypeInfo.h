//
// TypeInfo.h - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

#include "AutoList.h"
#include "Member.h"
#include "Method.h"

namespace dd
{
	template< typename T >
	class HasTypeInfo
	{
	public:
		HasTypeInfo()
		{
			T::RegisterType();
		}
	};

	class TypeInfo : public dd::AutoList<dd::TypeInfo>
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

		static TypeInfo* GetType( const dd::StringBase& name );

		template< typename T >
		static void Register( const char* name )
		{
			TypeInfo* type = GetType<T>();
			type->Init<T>( name, sizeof( T ) );
		}

		inline const dd::Vector<dd::MemberBase*>& GetMembers()
		{
			return m_members;
		}

	private:

		size_t m_size;
		dd::String m_name;
		dd::Vector<dd::MemberBase*> m_members;
		dd::Vector<dd::MethodBase*> m_methods;

		template< typename T >
		void Init( const char* name, size_t size )
		{
			if( m_size > 0 )
				return;

			m_size = size;
			m_name = name;

			T::RegisterMembers( m_members, m_methods );
		}
	};
}
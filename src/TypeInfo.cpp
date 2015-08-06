//
// TypeInfo.cpp - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"
#include "TypeInfo.h"

dd::TypeInfo::TypeInfo()
	: m_size( 0 )
{

}

dd::TypeInfo::~TypeInfo()
{
	for( dd::MemberBase* ptr : m_members )
		delete ptr;
}

dd::TypeInfo* dd::TypeInfo::GetType( const std::string& name )
{
	const dd::TypeInfo* ptr = dd::TypeInfo::Head();
	while( ptr != nullptr )
	{
		if( name == ptr->m_name )
			return const_cast<dd::TypeInfo*>( ptr );

		ptr = ptr->Next();
	}

	return nullptr;
}
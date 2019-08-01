//
// System.cpp
// Copyright (C) Sebastian Nordgren 
// September 24th 2018
//

#include "PCH.h"
#include "System.h"

namespace ddc
{
	System::System(const char* name) :
		m_name(name)
	{
	}

	bool System::CheckDuplicates( const dd::TypeInfo* component, DataUsage usage, DataCardinality cardinality, const char* name ) const
	{
		for( const DataRequest* req : m_requests )
		{
			if( req->Component() == *component &&
				req->Usage() == usage && 
				req->Cardinality() == cardinality && 
				req->Name() == name )
			{
				DD_ASSERT( false, "Duplicate DataRequest found!" );
				return false;
			}
		}

		return true;
	}

	void System::RequireTag( ddc::Tag tag, const char* name )
	{
		dd::String16 str;
		if( name != nullptr )
		{
			str = name;
		}

		for( TagRequest& req : m_tags )
		{
			if( req.Name == str )
			{
				req.Tags.set( (uint) tag );
				return;
			}
		}

		TagRequest req;
		req.Tags.set( (uint) tag );
		req.Name = str;

		m_tags.Add( req );
	}

	std::bitset<MAX_TAGS> System::GetRequiredTags( const char* name ) const
	{
		dd::String16 str;
		if( name != nullptr )
		{
			str = name;
		}

		for( TagRequest& req : m_tags )
		{
			if( req.Name == str )
			{
				return req.Tags;
			}
		}

		return std::bitset<MAX_TAGS>();
	}
}
//
// System.cpp
// Copyright (C) Sebastian Nordgren 
// September 24th 2018
//

#include "PrecompiledHeader.h"
#include "System.h"

namespace ddc
{
	bool System::CheckDuplicates( const ComponentType& component, DataUsage usage, DataCardinality cardinality, const char* name ) const
	{
		for( const DataRequest* req : m_requests )
		{
			if( req->Component() == component &&
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
}
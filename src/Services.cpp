//
// Services.cpp - A service registry to keep track of global instances of certain types.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PrecompiledHeader.h"

#include "Services.h"

namespace dd
{
	Services* Services::m_instance = nullptr;

	Services::Services()
	{

	}

	Services::~Services()
	{
		
	}

	void Services::Initialize()
	{
		DD_ASSERT( m_instance == nullptr );

		m_instance = new Services();
	}
}
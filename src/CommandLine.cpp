//
// CommandLine.cpp - Command line parser.
// Copyright (C) Sebastian Nordgren 
// September 29th 2015
//

#include "PrecompiledHeader.h"
#include "CommandLine.h"

namespace dd
{
	static String8 s_empty;

	CommandLine::CommandLine( char const* argv[], int argc )
	{
		for( int i = 1; i < argc; ++i )
		{
			String64 arg( argv[i] );
			
			if( arg[0] != '-' )
				continue;

			ArgumentPair& pair = m_args.Allocate();

			int separator = arg.Find( "=" );
			if( separator != -1 )
			{
				pair.Key = arg.Substring( 1, separator - 1 );
				pair.Value = arg.Substring( separator + 1 );
			}
			else
			{
				pair.Key = arg.Substring( 1 );
			}
		}
	}

	const String& CommandLine::GetValue( const char* key ) const
	{
		for( ArgumentPair& pair : m_args )
		{
			if( pair.Key == key )
				return pair.Value;
		}

		return s_empty;
	}

	bool CommandLine::Exists( const char* key ) const
	{
		for( ArgumentPair& pair : m_args )
		{
			if( pair.Key == key )
				return true;
		}

		return false;
	}
}
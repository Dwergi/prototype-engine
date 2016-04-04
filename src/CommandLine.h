//
// CommandLine.h - Command line parser.
// Copyright (C) Sebastian Nordgren 
// September 29th 2015
//

#pragma once

namespace dd
{
	class CommandLine
	{
	public:
		CommandLine( char const* argv[], int argc );

		bool Exists( const char* key ) const;
		const String& GetValue( const char* key ) const;

		BASIC_TYPE( CommandLine )

	private:

		struct ArgumentPair
		{
			String32 Key;
			String32 Value;
		};

		Vector<ArgumentPair> m_args;

		// only used for constructing the type info
		CommandLine() {}
	};
}
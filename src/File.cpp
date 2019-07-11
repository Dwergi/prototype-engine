//
// File.cpp - Wrapper around file handling.
// Copyright (C) Sebastian Nordgren 
// September 21st 2016
//

#include "PCH.h"
#include "File.h"

#include <fstream>

namespace dd
{
	std::filesystem::path File::s_dataRoot;

	void File::SetDataRoot( std::string root )
	{
		std::filesystem::path canonical_path(root);
		canonical_path = std::filesystem::canonical(canonical_path);

		if (!std::filesystem::is_directory(canonical_path))
		{
			DD_ASSERT(false, "Invalid data root given, not a directory: %s!", root.c_str());
			return;
		}

		if (!std::filesystem::exists(canonical_path))
		{
			DD_ASSERT(false, "Invalid data root given, doesn't exist: %s!", root.c_str());
			return;
		}

		s_dataRoot = canonical_path;
	}

	bool File::Exists( std::string file_path )
	{
		std::filesystem::path path( s_dataRoot );
		path.append( file_path );

		return !std::filesystem::is_directory( path ) && std::filesystem::exists( path );
	}

	File::File( std::string_view path )
	{
		DD_ASSERT( !std::filesystem::is_directory( path ) );

		m_path = s_dataRoot;
		m_path.append( path );
	}

	bool File::Read( byte* buffer, size_t length ) const
	{
		if( length < Size() )
		{
			return false;
		}

		std::ifstream stream( m_path );
		if( stream.bad() )
		{
			return false;
		}

		stream.read( reinterpret_cast<char*>( buffer ), length );
		return true;
	}

	bool File::Read( std::string& dst ) const
	{
		std::ifstream stream( m_path );
		if( stream.bad() )
		{
			return false;
		}

		size_t file_size = Size();

		dst.resize( file_size );
		stream.read( dst.data(), file_size );

		size_t i = dst.find( '\0' );
		if( i != std::string::npos )
		{
			dst.resize( i );
		}

		return true;
	}

	bool File::Write( const std::string& src ) const
	{
		std::ofstream stream( m_path );
		if( stream.bad() )
		{
			return false;
		}

		stream << src;
		return true;
	}

	bool File::Write( const byte* buffer, size_t length ) const
	{
		std::ofstream stream( m_path );
		if( stream.bad() )
		{
			return false;
		}

		stream.write( reinterpret_cast<const char*>( buffer ), length );
		return true;
	}
}

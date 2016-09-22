//
// File.cpp - Wrapper around file handling.
// Copyright (C) Sebastian Nordgren 
// September 21st 2016
//

#include "PrecompiledHeader.h"
#include "File.h"

#include "tinydir/tinydir.h"

namespace dd
{
	static String128 s_dataRoot;

	void File::SetDataRoot( const char* root )
	{
		wchar_t wpath[256];
		Buffer<wchar_t> buffer( wpath, 256 );

		String256 path( root );
		path.w_str( buffer );

		tinydir_dir dir;

		if( tinydir_open( &dir, wpath ) == -1 )
		{
			DD_ASSERT( false, "Invalid data root given!" );
			return; // don't set an invalid data root
		}
		
		s_dataRoot = root;
	}

	File* File::OpenDataFile( const char* path, Mode mode )
	{
		return OpenDataFile( String256( path ), mode );
	}

	File* File::OpenDataFile( const String& path, Mode mode )
	{
		String256 fullpath( s_dataRoot );
		fullpath += '/';
		fullpath += path;

		std::FILE* file_handle;
		errno_t error = fopen_s( &file_handle, fullpath.c_str(), (mode == Mode::Read ? "rb" : "wb") );
		if( error != 0 )
			return nullptr;

		File* out = new File( file_handle );
		return out;
	}

	int File::Read( byte* buffer, uint size )
	{
		DD_ASSERT( buffer != nullptr );

		size_t read = std::fread( buffer, sizeof( byte ), size, m_fileHandle );
		if( read < size )
		{
			if( !std::feof( m_fileHandle ) )
			{
				return -1;
			}
		}

		return (int) read;
	}

	int File::Write( const byte* buffer, uint size )
	{
		DD_ASSERT( buffer != nullptr );

		return 0;
	}

	File::~File()
	{
		DD_ASSERT( m_fileHandle != NULL );

		std::fclose( m_fileHandle );
	}

	File::File( std::FILE* handle )
		: m_fileHandle( handle )
	{
	}
}

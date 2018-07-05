//
// File.cpp - Wrapper around file handling.
// Copyright (C) Sebastian Nordgren 
// September 21st 2016
//

#include "PrecompiledHeader.h"
#include "File.h"

#include <filesystem>

namespace dd
{
	static String128 s_dataRoot;

	void File::SetDataRoot( const char* root )
	{
		std::filesystem::path path( root );

		if( !std::filesystem::is_directory( path ) || !std::filesystem::exists( path ) )
		{
			DD_ASSERT( false, "Invalid data root given!" );
			return; // don't set an invalid data root
		}

		s_dataRoot = root;
	}

	const char* File::GetDataRoot()
	{
		return s_dataRoot.c_str();
	}

	std::unique_ptr<File> File::OpenDataFile( const char* path, Mode mode )
	{
		return OpenDataFile( String256( path ), mode );
	}

	std::unique_ptr<File> File::OpenDataFile( const String& path, Mode mode )
	{
		String256 fullpath( s_dataRoot );
		fullpath += '/';
		fullpath += path;

		std::FILE* file_handle;
		errno_t error = fopen_s( &file_handle, fullpath.c_str(), (mode == Mode::Read ? "rb" : "wb") );
		if( error != 0 )
			return nullptr;

		return std::unique_ptr<File>( new File( file_handle ) );
	}

	int File::Size() const
	{
		std::fseek( m_fileHandle, 0L, SEEK_END );
		int size = std::ftell( m_fileHandle );
		std::rewind( m_fileHandle );
		return size;
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

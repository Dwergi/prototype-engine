//
// File.cpp - Wrapper around file handling.
// Copyright (C) Sebastian Nordgren 
// September 21st 2016
//

#include "PCH.h"
#include "File.h"

#include <cstdio>

namespace dd
{
	std::string File::s_dataRoot;

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

		s_dataRoot = canonical_path.string();
	}

	bool File::Exists( std::string file_path )
	{
		std::filesystem::path path( s_dataRoot );
		path /= file_path;

		return !std::filesystem::is_directory( path ) && std::filesystem::exists( path );
	}

	File::File( std::string_view relative_file )
	{
		std::filesystem::path path( s_dataRoot );
		path /= relative_file;

		DD_ASSERT(!std::filesystem::is_directory(path));

		m_path = path.string();
	}

	FILE* File::Open(const char* mode) const
	{
		FILE* file;
		errno_t err = fopen_s(&file, m_path.c_str(), mode);
		if (err != 0)
		{
			return nullptr;
		}

		return file;
	}

	size_t File::Read( Buffer<byte>& buffer ) const
	{
		if(buffer.SizeBytes() < Size())
		{
			return 0;
		}

		FILE* file = Open("rb");
		if (file == nullptr)
		{
			return 0;
		}

		size_t read = fread(buffer.Access(), 1, (size_t) buffer.SizeBytes(), file);
		fclose(file);
		return read;
	}

	Buffer<byte> File::ReadIntoBuffer() const
	{
		size_t file_size = Size();
		Buffer<byte> buffer(new byte[file_size], file_size);

		size_t read = Read(buffer);
		if (read == 0)
		{
			buffer.Delete();
		}
		
		return buffer;
	}

	size_t File::Read( std::string& dst ) const
	{
		FILE* file = Open("rb");
		if (file == nullptr)
		{
			return 0;
		}

		dst.resize(Size());

		size_t read = fread(dst.data(), 1, Size(), file);
		fclose(file);

		size_t i = dst.find( '\0' );
		if( i != std::string::npos )
		{
			dst.resize( i );
		}

		return dst.length();
	}

	bool File::Write( const std::string& src ) const
	{
		FILE* file = Open("wb");
		if (file == nullptr)
		{
			return 0;
		}

		size_t written = fwrite(src.c_str(), 1, src.size(), file);
		fclose(file);

		return written > 0;
	}

	bool File::Write( const IBuffer& buffer ) const
	{
		FILE* file = Open("wb");
		if (file == nullptr)
		{
			return 0;
		}

		size_t written = fwrite(buffer.GetVoid(), 1, buffer.SizeBytes(), file);
		fclose(file);

		return written > 0;
	}
}

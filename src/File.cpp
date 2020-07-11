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
	std::filesystem::path File::s_basePath;
	std::vector<std::filesystem::path> File::s_overridePaths;
	
	void File::SetBasePath(std::string_view path)
	{
		std::filesystem::path canonical_path(path);
		canonical_path = std::filesystem::canonical(canonical_path);

		if (!std::filesystem::is_directory(canonical_path))
		{
			DD_ASSERT(false, "Invalid base path given, not a directory: %s!", canonical_path.string().c_str());
			return;
		}

		if (!std::filesystem::exists(canonical_path))
		{
			DD_ASSERT(false, "Invalid base path given, doesn't exist: %s!", canonical_path.string().c_str());
			return;
		}

		s_basePath = canonical_path;
	}

	void File::AddOverridePath(std::string_view path)
	{
		std::filesystem::path canonical_path(s_basePath);
		canonical_path /= path;
		canonical_path = std::filesystem::canonical(canonical_path);

		if (!std::filesystem::is_directory(canonical_path))
		{
			DD_ASSERT(false, "Invalid base path given, not a directory: %s!", canonical_path.string().c_str());
			return;
		}

		if (!std::filesystem::exists(canonical_path))
		{
			DD_ASSERT(false, "Invalid base path given, doesn't exist: %s!", canonical_path.string().c_str());
			return;
		}

		// insert at front so that later added roots override earlier ones, which allows games to override any base file with their own version
		dd::push_front(s_overridePaths, canonical_path);
	}

	std::filesystem::path File::GetWritePath()
	{
		if (s_overridePaths.size() > 0)
		{
			return dd::last(s_overridePaths);
		}

		return s_basePath;
	}

	bool File::Exists(std::string_view file_path)
	{
		for (const std::filesystem::path& base : s_overridePaths)
		{
			std::filesystem::path path(base);
			path /= file_path;

			if (std::filesystem::exists(path) && !std::filesystem::is_directory(path))
			{
				return true;
			}
		}

		std::filesystem::path path(s_basePath);
		path /= file_path;

		return std::filesystem::exists(path) && !std::filesystem::is_directory(path);
	}

	File::File(std::string_view relative_file)
	{
		std::filesystem::path base_to_use;

		for (const std::filesystem::path& base : s_overridePaths)
		{
			std::filesystem::path path(base);
			path /= relative_file;

			if (std::filesystem::exists(path) && !std::filesystem::is_directory(path))
			{
				base_to_use = base;
				break;
			}
		}

		if (base_to_use.empty())
		{
			base_to_use = s_basePath;
		}

		std::filesystem::path path = base_to_use / relative_file;

		DD_ASSERT(!std::filesystem::is_directory(path));

		m_path = path;
	}

	FILE* File::Open(const char* mode) const
	{
		FILE* file;
		errno_t err = fopen_s(&file, m_path.string().c_str(), mode);
		if (err != 0)
		{
			return nullptr;
		}

		return file;
	}

	size_t File::Read(Buffer<byte>& buffer) const
	{
		if (buffer.SizeBytes() < Size())
		{
			return 0;
		}

		FILE* file = Open("rb");
		if (file == nullptr)
		{
			return 0;
		}

		size_t read = std::fread(buffer.Access(), 1, (size_t) buffer.SizeBytes(), file);
		std::fclose(file);
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

	size_t File::Read(std::string& dst) const
	{
		FILE* file = Open("rb");
		if (file == nullptr)
		{
			return 0;
		}

		dst.resize(Size());

		size_t read = std::fread(dst.data(), 1, Size(), file);
		std::fclose(file);

		size_t i = dst.find('\0');
		if (i != std::string::npos)
		{
			DD_ASSERT(false, "Resized");
			dst.resize(i);
		}

		return dst.length();
	}

	bool File::Write(std::string_view src) const
	{
		dd::ConstBuffer<char> buffer(src.data(), src.size());
		return Write(buffer);
	}

	bool File::Write(const IBuffer& buffer) const
	{
		FILE* file = Open("wb");
		if (file == nullptr)
		{
			return 0;
		}

		size_t written = std::fwrite(buffer.GetVoid(), 1, buffer.SizeBytes(), file);
		std::fclose(file);

		return written > 0;
	}
}

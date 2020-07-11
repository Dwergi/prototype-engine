//
// AngelScriptEngine.cpp - Wrapper around file handling.
// Copyright (C) Sebastian Nordgren 
// September 21st 2016
//

#pragma once

#include <filesystem>

namespace dd
{
	struct File
	{
		File(std::string_view relative_file);

		std::string Path() const { return m_path.string(); }

		size_t Read(std::string& dst) const;
		size_t Read(Buffer<byte>& buffer) const;
		Buffer<byte> ReadIntoBuffer() const;

		bool Write(std::string_view src) const;
		bool Write(const IBuffer& buffer) const;

		size_t Size() const { return std::filesystem::file_size(m_path); }

		static bool Exists(std::string_view relative_file);

		static void SetBasePath(std::string_view base);
		static void AddOverridePath(std::string_view root);
		static std::filesystem::path GetWritePath();

	private:
		std::filesystem::path m_path;

		static std::filesystem::path s_basePath;
		static std::vector<std::filesystem::path> s_overridePaths;

		FILE* Open(const char* mode) const;
	};
}

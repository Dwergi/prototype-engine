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
		File( std::string_view relative_file);

		std::string Path() const { return m_path; }

		size_t Read(std::string& dst) const;
		size_t Read(Buffer<byte>& buffer) const;
		Buffer<byte> ReadIntoBuffer() const;

		bool Write(const std::string& src) const;
		bool Write(const IBuffer& buffer) const;

		size_t Size() const { return std::filesystem::file_size( m_path ); }

		static bool Exists( std::string relative_file);

		static void SetDataRoot( std::string root );
		static std::string GetDataRoot() { return s_dataRoot; }

	private:
		std::string m_path;
		static std::string s_dataRoot;

		FILE* Open(const char* mode) const;
	};
}

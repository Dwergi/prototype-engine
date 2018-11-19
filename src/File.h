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
		File( std::string path );

		std::string Path() const { return m_path.string(); }

		bool Read( std::string& dst ) const;
		bool Read( byte* dst, size_t length ) const;

		bool Write( const std::string& src ) const;
		bool Write( const byte* src, size_t length ) const;

		size_t Size() const { return std::filesystem::file_size( m_path ); }

		static void SetDataRoot( std::string root );
		static std::string GetDataRoot() { return s_dataRoot.string(); }

	private:
		std::filesystem::path m_path;

		static std::filesystem::path s_dataRoot;
	};
}

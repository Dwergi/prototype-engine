//
// AngelScriptEngine.cpp - Wrapper around file handling.
// Copyright (C) Sebastian Nordgren 
// September 21st 2016
//

#pragma once

namespace dd
{
	class File
	{
	public:

		enum class Mode
		{
			Read,
			Write
		};
		
		// Create a file from the given path, rooted at the data root.
		// The file must be deleted by the caller.
		static std::unique_ptr<File> OpenDataFile( const char* path, Mode mode );
		static std::unique_ptr<File> OpenDataFile( const String& path, Mode mode );

		int Read( byte* buffer, uint size );
		int Write( const byte* buffer, uint size );

		int Size() const;

		static void SetDataRoot( const char* root );
		static const char* GetDataRoot();

		~File();

	private:

		File( std::FILE* handle );

		std::FILE* m_fileHandle;
	};
}

//
// StringBinding.cpp - Wrapper to register our implementation of strings with AngelScript.
// Copyright (C) Sebastian Nordgren 
// June 2nd 2016
//

#include "PCH.h"
#include "StringBinding.h"

#include "AngelScriptEngine.h"

namespace dd
{
	typedef DenseMap<const char*, String128> StringPool;
	const asPWORD STRING_POOL_HANDLE = 2001;

	String128 g_empty;

	const dd::String& StringFactory( asUINT length, const char* str )
	{
		// Each engine instance has its own string pool
		asIScriptContext* context = asGetActiveContext();
		if( context == nullptr )
		{
			// The string factory can only be called from a script
			DD_ASSERT( context != nullptr );
			return g_empty;
		}

		asIScriptEngine* engine = context->GetEngine();

		StringPool* pool = reinterpret_cast<StringPool*>( engine->GetUserData( STRING_POOL_HANDLE ) );
		if( pool == nullptr )
		{
			// The string pool hasn't been created yet, so we'll create it now
			asAcquireExclusiveLock();

			// Make sure the string pool wasn't created while we were waiting for the lock
			pool = reinterpret_cast<StringPool*>( engine->GetUserData( STRING_POOL_HANDLE ) );
			if( !pool )
			{
				pool = new StringPool();
				if( pool == nullptr )
				{
					context->SetException( "Out of memory" );
					asReleaseExclusiveLock();
					return g_empty;
				}

				engine->SetUserData( pool, STRING_POOL_HANDLE );
			}

			asReleaseExclusiveLock();
		}

		// We can't let other threads modify the pool while we query it
		asAcquireSharedLock();

		// First check if a string object hasn't been created already
		dd::String128* found = pool->Find( str );
		if( found != nullptr )
		{
			asReleaseSharedLock();
			return* found;
		}

		asReleaseSharedLock();

		// Acquire an exclusive lock so we can add the new string to the pool
		asAcquireExclusiveLock();

		// Make sure the string wasn't created while we were waiting for the exclusive lock
		found = pool->Find( str );
		if( found == nullptr )
		{
			// Create a new string object
			pool->Add( str, dd::String128( str, length ) );
			asReleaseExclusiveLock();
			return* pool->Find( str );
		}

		asReleaseExclusiveLock();
		return g_empty;
	}

	void CleanupEngineStringPool( asIScriptEngine* engine )
	{
		StringPool* pool = reinterpret_cast<StringPool*>(engine->GetUserData( STRING_POOL_HANDLE ) );
		if( pool )
			delete pool;
	}

	void ConstructString( String* thisPointer )
	{
		new (thisPointer) String128();
	}

	void CopyConstructString( const String& other, String* thisPointer )
	{
		new (thisPointer) String128( other );
	}

	void DestructString( String* thisPointer )
	{
		((String128*) thisPointer)->~String128();
	}

	String& AddAssignStringToString( const String& str, String& dest )
	{
		dest += str;
		return dest;
	}

	bool StringIsEmpty( const String& str )
	{
		return str.Length() == 0;
	}

	char* StringCharAt( unsigned int i, String& str )
	{
		if( i >= (unsigned int) str.Length() )
		{
			// Set a script exception
			asIScriptContext* ctx = asGetActiveContext();
			ctx->SetException( "Out of range" );

			// Return a null pointer
			return nullptr;
		}

		return &str[i];
	}

	// This function returns the index of the first position where the substring
	// exists in the input string. If the substring doesn't exist in the input
	// string -1 is returned.
	//
	// AngelScript signature:
	// int string::findFirst(const String& in sub, uint start = 0) const
	int StringFindFirst( const String& sub, asUINT start, const String& str )
	{
		return (int) str.Find( sub, start );
	}

	// AngelScript signature:
	// uint string::length() const
	asUINT StringLength( const String& str )
	{
		return (asUINT) str.Length();
	}

	// This function returns a string containing the substring of the input string
	// determined by the starting index and count of characters.
	//
	// AngelScript signature:
	// string string::substr(uint start = 0, int count = -1) const
	String128 StringSubString( asUINT start, int count, const String& str )
	{
		// Check for out-of-bounds
		String128 ret;
		if( start < (uint) str.Length() && count != 0 )
			ret = ((const String128&) str).Substring( start, count );

		return ret;
	}

	bool StringEquals( const String& lhs, const String& rhs )
	{
		return lhs == rhs;
	}

	#define UNUSED_VAR( x ) (void)(x)

	void RegisterString( AngelScriptEngine& engine )
	{
		int r = 0;
		UNUSED_VAR( r );

		asIScriptEngine* asEngine = engine.GetInternalEngine();

		// With C++11 it is possible to use asGetTypeTraits to automatically determine the correct flags to use
		r = asEngine->RegisterObjectType( "string", sizeof( dd::String128 ), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK ); DD_ASSERT( r >= 0 );

		// Register the string factory
		r = asEngine->RegisterStringFactory( "const string&", asFUNCTION( StringFactory ), asCALL_CDECL ); DD_ASSERT( r >= 0 );

		// Register the cleanup callback for the string pool
		asEngine->SetEngineUserDataCleanupCallback( CleanupEngineStringPool, STRING_POOL_HANDLE );

		// Register the object operator overloads
		r = asEngine->RegisterObjectBehaviour( "string", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION( ConstructString ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectBehaviour( "string", asBEHAVE_CONSTRUCT, "void f(const string& in)", asFUNCTION( CopyConstructString ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectBehaviour( "string", asBEHAVE_DESTRUCT, "void f()", asFUNCTION( DestructString ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectMethod( "string", "string& opAssign(const string& in)", asMETHODPR( String, operator =, (const String&), String& ), asCALL_THISCALL ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectMethod( "string", "string& opAddAssign(const string& in)", asFUNCTION( AddAssignStringToString ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );

		r = asEngine->RegisterObjectMethod( "string", "bool opEquals(const string& in) const", asFUNCTIONPR( StringEquals, (const String&, const String&), bool ), asCALL_CDECL_OBJFIRST ); DD_ASSERT( r >= 0 );

		// The string length can be accessed through methods or through virtual property
		r = asEngine->RegisterObjectMethod( "string", "uint length() const", asFUNCTION( StringLength ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectMethod( "string", "uint get_length() const", asFUNCTION( StringLength ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );

		r = asEngine->RegisterObjectMethod( "string", "bool isEmpty() const", asFUNCTION( StringIsEmpty ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );

		// Register the index operator, both as a mutator and as an inspector
		// Note that we don't register the operator[] directly, as it doesn't do bounds checking
		r = asEngine->RegisterObjectMethod( "string", "uint8& opIndex(uint)", asFUNCTION( StringCharAt ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectMethod( "string", "const uint8& opIndex(uint) const", asFUNCTION( StringCharAt ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );

		// Utilities
		r = asEngine->RegisterObjectMethod( "string", "string substr(uint start = 0, uint count = -1) const", asFUNCTION( StringSubString ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
		r = asEngine->RegisterObjectMethod( "string", "int findFirst(const string& in, uint start = 0) const", asFUNCTION( StringFindFirst ), asCALL_CDECL_OBJLAST ); DD_ASSERT( r >= 0 );
	}
}
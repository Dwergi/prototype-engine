//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "JSONSerializer.h"

namespace dd
{
	ScopedJSONObject::ScopedJSONObject( JSONSerializer& serializer )
		: m_host( serializer )
	{
		m_previous = m_host.m_currentObject;
		m_host.m_currentObject = this;

		Indent();

		m_host.m_stream.Write( "{\n", 2 );

		++m_host.m_indent;
	}

	ScopedJSONObject::~ScopedJSONObject()
	{
		--m_host.m_indent;
		Indent();

		m_host.m_stream.Write( "}\n", 2 );

		m_host.m_currentObject = m_previous;
	}

	void ScopedJSONObject::Indent() const
	{
		for( uint i = 0; i < m_host.m_indent; ++i ) 
			m_host.m_stream.Write( "\t", 1 );
	}
	//===================================================================================

	JSONSerializer::JSONSerializer( WriteStream& stream )
		: m_currentObject( nullptr ),
		m_stream( stream )
	{

	}

	JSONSerializer::JSONSerializer( String& buffer )
		: m_currentObject( nullptr ),
		m_stream( buffer ),
		m_indent( 0 )
	{
		
	}

	JSONSerializer::~JSONSerializer()
	{

	}

	void JSONSerializer::Indent()
	{
		if( m_currentObject != nullptr )
			m_currentObject->Indent();
	}

	void JSONSerializer::AddKey( const String& key )
	{
		Indent();
		Serialize( key );
		m_stream.Write( " : ", 3 );
	}

	void JSONSerializer::AddString( const char* key, const String& value, bool last )
	{
		AddKey( String16( key ) );
		Serialize( value );

		if( !last )
			m_stream.Write( ",", 1 );

		m_stream.Write( "\n", 1 );
	}

	bool JSONSerializer::Serialize( Variable var )
	{
		const TypeInfo* type = var.Type();
		
		ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->HasCustomSerializers() )
		{
			type->SerializeCustom( Serialize::Mode::JSON, m_stream, var );
		}
		else
		{
			ASSERT( type->GetMembers().Size() > 0 );

			// composite object
			ScopedJSONObject obj( *this );
			AddString( "type", type->Name() );

			AddKey( String16( "members" ) );
			m_stream.Write( "\n", 1 );

			{
				ScopedJSONObject members( *this );

				uint index = 0;
				uint member_count = type->GetMembers().Size();
				for( const Member& member : type->GetMembers() )
				{
					AddKey( member.Name() );

					void* data = PointerAdd( var.Data(), member.Offset() );
					
					if( !Serialize( Variable( member.Type(), data ) ) )
						return false;

					if( index < (member_count - 1) )
						m_stream.Write( ",", 1 );

					m_stream.Write( "\n", 1 );

					++index;
				}
			}
		}

		return true;
	}
	//===================================================================================

	struct JSONKeyValuePair
	{
		String32 Key;
		String32 Value;

		bool HasChildren;
	};

	JSONDeserializer::JSONDeserializer( ReadStream& stream )
		: m_stream( stream )
	{

	}

	JSONDeserializer::JSONDeserializer( const String& buffer )
		: m_stream( buffer )
	{

	}

	JSONDeserializer::~JSONDeserializer()
	{

	}

	bool IsWhiteSpace( char c )
	{
		if( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
			return true;

		return false;
	}

	void ReadUntil( ReadStream& stream, char until, String& result )
	{
		if( stream.Remaining() == 0 )
			return;

		char c = stream.PeekByte();
		while( stream.Remaining() > 0 
			&& c != until )
		{
			result += c;
			stream.ReadByte();
			c = stream.PeekByte();
		}
	}

	void ReadUntilWhitespaceOr( ReadStream& stream, char until, String& result )
	{
		if( stream.Remaining() == 0 )
			return;

		char c = stream.PeekByte();
		while( stream.Remaining() > 0
			&& c != until
			&& !IsWhiteSpace( c ) )
		{
			result += c;
			stream.ReadByte();
			c = stream.PeekByte();
		}
	}

	void SkipWhitespace( ReadStream& stream )
	{
		while( stream.Remaining() > 0
			&& IsWhiteSpace( stream.PeekByte() ) )
		{
			stream.ReadByte();
		}
	}

	void SkipUntil( ReadStream& stream, char until )
	{
		while( stream.Remaining() > 0
			&& stream.PeekByte() != until )
		{
			stream.ReadByte();
		}
	}

	void ReadNextPair( ReadStream& stream, JSONKeyValuePair& kvp )
	{
		char current = stream.ReadByte();

		SkipWhitespace( stream );

		// expecting a name here
		current = stream.ReadByte();
		ASSERT( current == '"' );

		ReadUntil( stream, '"', kvp.Key );

		current = stream.ReadByte();
		ASSERT( current == '"' );

		SkipUntil( stream, ':' );

		current = stream.ReadByte();
		ASSERT( current == ':' );

		SkipWhitespace( stream );
			
		current = stream.PeekByte();
		if( current == '{' )
		{
			kvp.HasChildren = true;
		}
		else if( current == '"' )
		{
			// serialized as string
			kvp.Value += stream.ReadByte();

			ReadUntil( stream, '"', kvp.Value );

			current = stream.ReadByte();
			kvp.Value += current;
			ASSERT( current == '"' );
		}
		else
		{
			ReadUntilWhitespaceOr( stream, ',', kvp.Value );

			current = stream.ReadByte();
			ASSERT( current == ',' );
		}
	}

	bool JSONDeserializer::Deserialize( Variable var )
	{
		const TypeInfo* type = var.Type();
		
		ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->HasCustomSerializers() )
			type->DeserializeCustom( Serialize::Mode::JSON, m_stream, var );
		else
		{
			ASSERT( type->GetMembers().Size() > 0 );

			// composite object
			char c = m_stream.ReadByte();

			ASSERT( c == '{' );

			JSONKeyValuePair pair;
			ReadNextPair( m_stream, pair );

			ASSERT( pair.Key == "type" );
			
			{
				String64 strType;
				JSONDeserializer nested( pair.Value );
				nested.Deserialize( strType );

				ASSERT( strType == type->Name() );
			}

			if( pair.HasChildren )
			{
				c = m_stream.ReadByte();
				ASSERT( c == '{' );


			}
		}

		return true;
	}
}
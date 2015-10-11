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

		m_host.m_stream.WriteByte( '}' );

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
			m_stream.WriteByte( ',' );

		m_stream.WriteByte( '\n' );
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
			ASSERT( type->Members().Size() > 0 );

			// composite object
			ScopedJSONObject obj( *this );
			AddString( "type", type->Name() );

			AddKey( String16( "members" ) );
			m_stream.WriteByte( '\n' );

			{
				ScopedJSONObject members( *this );

				uint index = 0;
				uint member_count = type->Members().Size();
				for( const Member& member : type->Members() )
				{
					AddKey( member.Name() );

					void* data = PointerAdd( var.Data(), member.Offset() );

					if( !member.Type()->IsPOD() )
						m_stream.WriteByte( '\n' );

					if( !Serialize( Variable( member.Type(), data ) ) )
						return false;

					if( index < (member_count - 1) )
						m_stream.WriteByte( ',' );

					m_stream.WriteByte( '\n' );

					++index;
				}
			}

			m_stream.WriteByte( '\n' );
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
			&& !isspace( c ) )
		{
			result += c;
			stream.ReadByte();
			c = stream.PeekByte();
		}
	}

	void SkipWhitespace( ReadStream& stream )
	{
		while( stream.Remaining() > 0
			&& isspace( stream.PeekByte() ) )
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
		kvp.Key.Clear();
		kvp.Value.Clear();
		kvp.HasChildren = false;

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
		else if( current == '[' )
		{
			// lists
			ReadUntil( stream, ']', kvp.Value );
			kvp.Value += stream.ReadByte();
		}
		else
		{
			ReadUntilWhitespaceOr( stream, ',', kvp.Value );
		}

		SkipWhitespace( stream );
	}

	bool JSONDeserializer::Deserialize( Variable var )
	{
		const TypeInfo* type = var.Type();
		
		ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->HasCustomSerializers() )
		{
			type->DeserializeCustom( Serialize::Mode::JSON, m_stream, var );
		}
		else
		{
			ASSERT( type->Members().Size() > 0 );

			// composite object
			char c = m_stream.ReadByte();
			ASSERT( c == '{' );

			JSONKeyValuePair pair;

			// check the type
			{			
				ReadNextPair( m_stream, pair );

				ASSERT( pair.Key == "type" );

				{
					String64 strType;
					JSONDeserializer nested( pair.Value );
					nested.Deserialize( strType );

					ASSERT( strType == type->Name() );
				}
			}

			// has to have a "members" entry
			{
				ASSERT( m_stream.ReadByte() == ',' );

				ReadNextPair( m_stream, pair );

				ASSERT( pair.Key == "members" && pair.HasChildren );
				ASSERT( m_stream.ReadByte() == '{' );
			}

			for( Member& member : type->Members() )
			{
				ReadNextPair( m_stream, pair );

				ASSERT( pair.Key == member.Name() );

				Variable member_var( member.Type(), PointerAdd( var.Data(), member.Offset() ) );

				if( pair.HasChildren )
				{
					// having children means there's potentially a lot of data for this pair, so pass in the stream
					JSONDeserializer nested( m_stream );
					nested.Deserialize( member_var );

					// skip ahead to wherever the nested serializer called it quits
					uint offset = nested.m_stream.Offset();
					m_stream.Advance( offset - m_stream.Offset() );

					SkipWhitespace( m_stream );
				}
				else
				{
					// just deserialize the value
					JSONDeserializer nested( pair.Value );
					nested.Deserialize( member_var );
				}

				char c = m_stream.ReadByte();
				ASSERT( c == ',' || c == '}' );
			}
		}

		return true;
	}
}
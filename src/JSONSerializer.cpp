//
// JSONSerializer.h - Serialization! To JSON!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#include "PrecompiledHeader.h"
#include "JSONSerializer.h"

#include "arduinojson/ArduinoJson.h"

namespace dd
{
	class JSONPrint 
		: public ArduinoJson::Print
	{
	public:
		JSONPrint( WriteStream& stream )
			: m_stream( stream ) {}

		virtual size_t write( uint8_t c ) override
		{
			m_stream.WriteByte( c );
			return 1;
		}

	private:
		WriteStream& m_stream;
	};

#define POD( T ) \
	if( type == GET_TYPE( T ) ) \
		return JsonVariant( var.GetValue<T>() );

	JsonVariant GetPODVariant( Variable var, JsonBuffer& buffer )
	{
		const TypeInfo* type = var.Type();
		DD_ASSERT( type->IsRegistered() );

		POD( bool );
		POD( int );
		POD( int8 );
		POD( int16 );
		POD( int32 );
		POD( int64 );
		POD( uint );
		POD( uint8 );
		POD( uint16 );
		POD( uint32 );
		POD( uint64 );
		POD( float );
		POD( double );

		return JsonVariant();
	}

#undef POD

	JSONSerializer::JSONSerializer( WriteStream& stream )
		: m_stream( stream )
	{
	}

	JSONSerializer::~JSONSerializer()
	{
	}

	JsonVariant GetVariant( Variable var, JsonBuffer& buffer );

	JsonVariant GetArrayVariant( Variable var, JsonBuffer& buffer )
	{
		const TypeInfo* type = var.Type();
		DD_ASSERT( type->IsRegistered() );

		JsonArray& arr = buffer.createArray();

		uint size = type->ContainerSize( var.Data() );
		for( uint i = 0; i < size; ++i )
		{
			void* elem = type->ElementAt( var.Data(), i );

			Variable elem_var( type->ContainedType(), elem );

			arr.add( GetVariant( elem_var, buffer ) );
		}

		return JsonVariant( arr );
	}

	JsonVariant GetObjectVariant( Variable var, JsonBuffer& buffer )
	{
		const TypeInfo* type = var.Type();
		DD_ASSERT( type->IsRegistered() );

		JsonObject& root = buffer.createObject();

		root["type"] = buffer.strdup( type->FullTypeName().c_str() );
		JsonObject& members = root.createNestedObject( "members" );

		for( const Member& member : type->Members() )
		{
			Variable member_var( var, member );
			members[member.Name().c_str()] = GetVariant( member_var, buffer );
		}

		return JsonVariant( root );
	}

	//
	// Central decision making for what type we're actually serializing.
	// Can be used recursively to fill out arrays and objects, which is nice.
	//
	JsonVariant GetVariant( Variable var, JsonBuffer& buffer )
	{
		const TypeInfo* type = var.Type();
		DD_ASSERT( type->IsRegistered() );

		if( type->IsPOD() )
		{
			return GetPODVariant( var, buffer );
		}
		else if( type->IsContainer() )
		{
			return GetArrayVariant( var, buffer );
		}
		else if( type->IsDerivedFrom( GET_TYPE( String ) ) )
		{
			return JsonVariant( buffer.strdup( var.GetValue<String>().c_str() ) );
		}
		else if( type == GET_TYPE( SharedString ) )
		{
			return JsonVariant( buffer.strdup( var.GetValue<SharedString>().c_str() ) );
		}
		else if( type->Members().Size() > 0 )
		{
			return GetObjectVariant( var, buffer );
		}

		return JsonVariant();
	}

	bool JSONSerializer::Serialize( Variable var )
	{
		DynamicJsonBuffer buffer;
		JsonVariant root = GetVariant( var, buffer );

		JSONPrint printer( m_stream );
		root.prettyPrintTo( printer );

		return root.success();
	}

	JSONDeserializer::JSONDeserializer( ReadStream& stream )
		: m_stream( stream )
	{

	}

	JSONDeserializer::~JSONDeserializer()
	{

	}

	bool JSONDeserializer::Deserialize( Variable var )
	{
		return true;
	}
	//===================================================================================
	/*
	bool JSONSerializer::Serialize( Variable var )
	{
		const TypeInfo* type = var.Type();
		
		DD_ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->HasCustomSerializers() )
		{
			type->SerializeCustom( Serialize::Mode::JSON, m_stream, var );
		}
		else
		{
			DD_ASSERT( type->Members().Size() > 0 );

			// composite object
			ScopedJSONObject obj( *this );
			AddString( "type", type->FullTypeName() );

			AddKey( String16( "members" ) );
			m_stream.WriteByte( '\n' );

			{
				ScopedJSONObject members( *this );

				uint index = 0;
				uint member_count = type->Members().Size();
				for( const Member& member : type->Members() )
				{
					AddKey( member.Name() );

					if( !member.Type()->IsPOD() )
						m_stream.WriteByte( '\n' );

					if( !Serialize( Variable( var, member ) ) )
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
		DD_ASSERT( current == '"' );

		ReadUntil( stream, '"', kvp.Key );

		current = stream.ReadByte();
		DD_ASSERT( current == '"' );

		SkipUntil( stream, ':' );

		current = stream.ReadByte();
		DD_ASSERT( current == ':' );

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
			DD_ASSERT( current == '"' );
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
		
		DD_ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->HasCustomSerializers() )
		{
			type->DeserializeCustom( Serialize::Mode::JSON, m_stream, var );
		}
		else
		{
			DD_ASSERT( type->Members().Size() > 0 );

			// composite object
			char c = m_stream.ReadByte();
			DD_ASSERT( c == '{' );

			JSONKeyValuePair pair;

			// check the type
			{			
				ReadNextPair( m_stream, pair );

				DD_ASSERT( pair.Key == "type" );

				{
					String128 strType;
					JSONDeserializer nested( pair.Value );
					nested.Deserialize( strType );

					DD_ASSERT( strType == type->FullTypeName() );
				}
			}

			// has to have a "members" entry
			{
				DD_ASSERT( m_stream.ReadByte() == ',' );

				ReadNextPair( m_stream, pair );

				DD_ASSERT( pair.Key == "members" && pair.HasChildren );
				DD_ASSERT( m_stream.ReadByte() == '{' );
			}

			for( Member& member : type->Members() )
			{
				ReadNextPair( m_stream, pair );

				DD_ASSERT( pair.Key == member.Name() );

				Variable member_var( var, member );

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
				DD_ASSERT( c == ',' || c == '}' );
			}
		}

		return true;
	}*/
}
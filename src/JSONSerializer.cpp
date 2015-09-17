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

	void JSONSerializer::Serialize( Variable var )
	{
		const TypeInfo* type = var.Type();
		
		ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return;

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
					Serialize( Variable( member.Type(), data ) );

					if( index < (member_count - 1) )
						m_stream.Write( ",", 1 );

					m_stream.Write( "\n", 1 );

					++index;
				}
			}
		}
	}

	JSONDeserializer::JSONDeserializer( ReadStream& stream )
	{

	}

	JSONDeserializer::JSONDeserializer( const String& buffer )
	{

	}

	JSONDeserializer::~JSONDeserializer()
	{

	}

	void JSONDeserializer::Deserialize( Variable var )
	{

	}
}
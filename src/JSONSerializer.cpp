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

	template <typename T>
	bool SetPOD( Variable& var, JsonVariant& variant )
	{
		if( !variant.is<T>() )
			return false;

		var.GetValue<T>() = variant.as<T>();
		return true;
	}

#define POD( T ) \
	if( var.Type() == GET_TYPE( T ) ) \
		return SetPOD<T>( var, variant );

	bool SetPODFromVariant( Variable& var, JsonVariant& variant )
	{
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

		return false;
	}

#undef POD

	bool JSONDeserializer::SetArrayFromVariant( Variable& var, JsonVariant& variant )
	{
		const TypeInfo* type = var.Type();

		// delete and new the container to empty it
		type->PlacementDelete( var.Data() );
		type->PlacementNew( var.Data() );

		const TypeInfo* contained = type->ContainedType();

		byte* buffer = new byte[contained->Size()];

		JsonArray& arr = variant.as<JsonArray>();

		for( size_t i = 0; i < arr.size(); ++i )
		{
			contained->PlacementNew( buffer );

			Variable element( contained, buffer );
			if( !SetFromVariant( element, arr.get( i ) ) )
			{
				delete[] buffer;
				return false;
			}

			type->InsertElement( var.Data(), i, buffer );

			contained->PlacementDelete( buffer );
		}

		delete[] buffer;
		return true;
	}

	bool JSONDeserializer::SetObjectFromVariant( Variable& var, JsonVariant& variant )
	{
		const TypeInfo* type = var.Type();

		JsonObject& obj = variant.as<JsonObject>();

		const char* typeName = obj["type"];
		DD_ASSERT( var.Type()->FullTypeName() == typeName );
		if( var.Type()->FullTypeName() != typeName )
			return false;

		JsonObject& members = obj["members"];
		for( const Member& member : type->Members() )
		{
			Variable member_var( var, member );

			JsonVariant& member_variant = members.get( member.Name().c_str() );
			if( !member_variant.success() )
				return false;

			if( !SetFromVariant( member_var, member_variant ) )
				return false;
		}

		return true;
	}

	//
	// Central decision making for what type we're actually deserializing.
	// 
	bool JSONDeserializer::SetFromVariant( Variable& var, JsonVariant& variant )
	{
		const TypeInfo* type = var.Type();

		if( type->IsDerivedFrom( GET_TYPE( String ) ) )
		{
			if( !variant.is<const char*>() )
				return false;

			var.GetValue<String>() = variant.as<const char*>();
			return true;
		}
		else if( type == GET_TYPE( SharedString ) )
		{
			if( !variant.is<const char*>() )
				return false;

			var.GetValue<SharedString>() = variant.as<const char*>();
			return true;
		}
		else if( type->IsContainer() )
		{
			return SetArrayFromVariant( var, variant );
		}
		else if( type->IsPOD() )
		{
			return SetPODFromVariant( var, variant );
		}
		else if( type->Members().Size() > 0 )
		{
			return SetObjectFromVariant( var, variant );
		}
		
		return false;
	}

	bool JSONDeserializer::Deserialize( Variable var )
	{
		const TypeInfo* type = var.Type();

		DD_ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		DynamicJsonBuffer buffer;
		JsonVariant variant = buffer.parse( (char*) m_stream.Data() );
		if( !variant.success() )
			return false;

		return SetFromVariant( var, variant );
	}
	//===================================================================================
}
//
// Serializers.h - Serialization!
// Copyright (C) Sebastian Nordgren 
// September 10th 2015
//

#pragma once

namespace dd
{
	const void* PointerAdd( const void* base, uint offset );
	void* PointerAdd( void* base, uint offset );

	namespace Serialize
	{
		enum class Mode : uint
		{
			STRING = 0,
			BINARY = 1,
			BASE64 = 2,
			JSON = 4
		};

		void SerializeString( Mode mode, WriteStream& dst, Variable src );
		void DeserializeString( Mode mode, ReadStream& src, Variable dst );

		template<typename T>
		void CopyPOD( T value, Stream& out )
		{
			memcpy( out, &value, sizeof( T ) );
		}

		template<typename T>
		void SerializePOD( Mode mode, WriteStream& dst, const T& src )
		{
			SerializePOD<RemoveQualifiers<T>::type>( mode, dst, Variable( src ) );
		}

		template<typename T>
		void SerializePOD( Mode mode, WriteStream& dst, Variable src )
		{
			ASSERT( "Should never hit this!");
		}

		template<>
		void SerializePOD<int>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<char>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<int16>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<int64>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<uint>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<byte>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<uint16>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<uint64>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<float>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<double>( Mode mode, WriteStream& dst, Variable src );
		template<>
		void SerializePOD<char*>( Mode mode, WriteStream& dst, Variable src );

		template<typename T>
		void DeserializePOD( Mode mode, ReadStream& src, Variable dst )
		{
			ASSERT( "Should never hit this!");
		}

		template<>
		void DeserializePOD<int>( Mode mode, ReadStream& dst, Variable src );
		template<>
		void DeserializePOD<char>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<int16>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<int64>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<uint>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<byte>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<uint16>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<uint64>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<float>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<double>( Mode mode, ReadStream& src, Variable dst );
		template<>
		void DeserializePOD<char*>( Mode mode, ReadStream& src, Variable dst );

		template<typename T>
		void SerializeContainer( Mode mode, WriteStream& dst, Variable src )
		{
			T& container = src.GetValue<T>();

			if( mode == Mode::JSON )
			{
				JSONSerializer json( dst );

				dst.Write( "[", 1 );

				bool first = true;
				for( auto& elem : container )
				{
					if( !first )
						dst.Write( ", ", 1 );

					json.Serialize( Variable( elem ) );

					first = false;
				}

				dst.Write( "]", 1 );
			}
		}

		template<typename T>
		void DeserializeContainer( Mode mode, ReadStream& src, Variable dst )
		{

		}
	}
}
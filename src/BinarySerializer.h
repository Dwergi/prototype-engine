//
// BinarySerializer.h - Serialization! To Binary!
// Copyright (C) Sebastian Nordgren 
// September 16th 2016
//

#pragma once

#include "Serialization.h"

namespace dd
{
	class BitField
	{
	public:

		BitField( uint size );
		
		void Set( uint index, bool value );
		bool Get( uint index ) const;

		void Write( WriteStream& stream ) const;
		void Read( ReadStream& stream );

	private:

		uint64 m_field;
		uint m_size;
	};
	//===================================================================================

	class BinarySerializer : public SerializerBase
	{
	public:

		BinarySerializer( WriteStream& stream );

		virtual bool Serialize( Variable var ) override;

		template <typename T>
		bool Serialize( const T& value );

	private:

		WriteStream& m_stream;
	};
	//===================================================================================

	class BinaryDeserializer : public DeserializerBase
	{
	public:

		BinaryDeserializer( ReadStream& stream );

		virtual bool Deserialize( Variable var ) override;

		template <typename T>
		bool Deserialize( const T& value );

	private:

		ReadStream& m_stream;
	};
	//===================================================================================

	template <typename T>
	bool BinarySerializer::Serialize( const T& obj )
	{
		return Serialize( Variable( GET_TYPE( T ), const_cast<T*>(&obj) ) );
	}
	//===================================================================================

	template <typename T>
	bool BinaryDeserializer::Deserialize( const T& obj )
	{
		return Deserialize( Variable( GET_TYPE( T ), const_cast<T*>(&obj) ) );
	}
}
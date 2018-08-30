//
// BinarySerializer.h - Serialization! To Binary!
// Copyright (C) Sebastian Nordgren 
// September 16th 2016
//

#include "PrecompiledHeader.h"
#include "BinarySerializer.h"

namespace dd
{
	BitField::BitField( uint size )
		: m_field( 0 ),
		m_size( size )
	{
		DD_ASSERT( size <= 64, "BitField cannot support fields larger than 64 bits! Why do you have that many members?!" );
	}

	void BitField::Set( uint index, bool value )
	{
		DD_ASSERT( index <= m_size, "Index outside of declared BitField size!" );

		uint64 mask = ((uint64) 1) << (63 - index);

		if( value )
			m_field |= mask;
		else
			m_field &= ~mask;
	}

	bool BitField::Get( uint index ) const
	{
		DD_ASSERT( index <= m_size, "Index outside of declared BitField size!" );

		uint64 mask = ((uint64) 1) << (63 - index);
		return (m_field & mask) != 0;
	}

	void BitField::Write( WriteStream& stream ) const
	{
		// TODO: This could write only m_size bits...
		stream.WritePOD( m_field );
	}

	void BitField::Read( ReadStream& stream )
	{
		m_field = stream.ReadPOD<uint64>();
	}
	//===================================================================================

	bool WriteValue( Variable var, WriteStream& stream )
	{
		const TypeInfo* type = var.Type();

		if( type->IsPOD() )
		{
			stream.Write( var.Data(), type->Size() );
			return true;
		}
		else if( type->IsDerivedFrom( DD_TYPE( String ) ) )
		{
			stream.Write( var.GetValue<String>().c_str() );
			return true;
		}
		else if( type == DD_TYPE( SharedString ) )
		{
			stream.Write( var.GetValue<SharedString>().c_str() );
			return true;
		}

		return false;
	}

	BinarySerializer::BinarySerializer( WriteStream& stream )
		: m_stream( stream )
	{
	}

	bool BinarySerializer::Serialize( Variable var )
	{
		const TypeInfo* type = var.Type();

		DD_ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->Members().Size() > 0 )
		{
			BitField modified( type->Members().Size() );
			uint index = 0;
			for( const Member& member : type->Members() )
			{
				// TODO: this should check if the value has changed and only write a 1 if it has
				modified.Set( index, true );
				++index;
			}

			// write the bitfield
			modified.Write( m_stream );

			index = 0;
			for( const Member& member : type->Members() )
			{
				// only serialize if the member has actually been modified
				if( modified.Get( index ) )
				{
					Variable member_var( var, member );
					if( !Serialize( member_var ) )
						return false;
				}

				++index;
			}
		}
		else if( type->IsContainer() )
		{
			uint size = type->ContainerSize( var.Data() );
			m_stream.WritePOD( size );

			for( uint i = 0; i < size; ++i )
			{
				Variable element( type->ContainedType(), type->ElementAt( var.Data(), i ) );
				if( !Serialize( element ) )
					return false;
			}
		}
		else
		{
			return WriteValue( var, m_stream );
		}

		return true;
	}
	//===================================================================================

	bool ReadValue( Variable var, ReadStream& stream )
	{
		const TypeInfo* type = var.Type();

		if( type->IsPOD() )
		{
			stream.Read( var.Data(), type->Size() );
			return true;
		}
		else if( type->IsDerivedFrom( DD_TYPE( String ) ) )
		{
			stream.Read( var.GetValue<String>() );
			return true;
		}
		else if( type == DD_TYPE( SharedString ) )
		{
			String256 str;
			stream.Read( str );

			var.GetValue<SharedString>() = SharedString( str );
			return true;
		}

		return false;
	}

	BinaryDeserializer::BinaryDeserializer( ReadStream& stream )
		: m_stream( stream )
	{

	}

	bool BinaryDeserializer::Deserialize( Variable var )
	{
		const TypeInfo* type = var.Type();

		DD_ASSERT( type->IsRegistered() );
		if( !type->IsRegistered() )
			return false;

		if( type->Members().Size() > 0 )
		{
			BitField modified( type->Members().Size() );
			modified.Read( m_stream );

			uint index = 0;
			for( const Member& member : type->Members() )
			{
				// only serialize if the member has actually been modified
				if( modified.Get( index ) )
				{
					Variable member_var( var, member );
					if( !Deserialize( member_var ) )
						return false;
				}

				++index;
			}
		}
		else if( type->IsContainer() )
		{
			// delete and new the container to empty it
			type->PlacementDelete( var.Data() );
			type->PlacementNew( var.Data() );

			const TypeInfo* contained = type->ContainedType();

			byte* buffer = new byte[contained->Size()];

			uint size = m_stream.ReadPOD<uint>();
			for( uint i = 0; i < size; ++i )
			{
				contained->PlacementNew( buffer );

				Variable element( contained, buffer );
				if( !Deserialize( element ) )
				{
					delete[] buffer;
					return false;
				}

				type->InsertElement( var.Data(), i, buffer );

				contained->PlacementDelete( buffer );
			}

			delete[] buffer;
		}
		else
		{
			return ReadValue( var, m_stream );
		}

		return false;
	}
}
#pragma once

namespace dd
{
	//
	// StackArray is, as the name implies, a static array that is allocated on the stack. Useful for temporary passing around of values. 
	//
	template< typename TValue, int MaxSize >
	class StackArray
	{
	public:

		StackArray()
			: m_back( 0 )
		{
		}

		template< int OtherSize >
		StackArray<TValue, MaxSize>& operator=( const StackArray<TValue, OtherSize>& other )
		{
			ASSERT( other.Size() <= MaxSize );

			// clear, then push the entire other array
			Clear();
			PushAll( other );

			return *this;
		}

		const TValue& operator[]( int index ) const
		{
			ASSERT( index < m_back, "Indexing unallocated memory!" );

			return m_data[ index ];
		}

		StackArray<TValue, MaxSize>& operator+=( const TValue& value )
		{
			Push( value );

			return *this;
		}

		int Size() const
		{
			return m_back;
		}

		void Push( const TValue& value )
		{
			ASSERT( m_back < MaxSize );

			m_data[ m_back ] = value;
			++m_back;
		}

		template< int OtherSize >
		void PushAll( const StackArray<TValue, OtherSize>& other )
		{
			ASSERT( other.Size() <= MaxSize - m_back );

			int count = other.Size();
			for( int i = 0; i < count; ++i )
			{
				Push( other[ i ] );
			}
		}

		TValue Pop()
		{
			ASSERT( m_back > 0 );

			--m_back;

			return m_data[ m_back ];
		}

		void Clear()
		{
			m_back = 0;
		}

		bool Contains( const TValue& val )
		{
			return IndexOf( val ) != -1;
		}

		int IndexOf( const TValue& val )
		{
			for( int i = 0; i < m_back; ++i )
			{
				if( m_data[ i ] == val )
					return i;
			}

			return -1;
		}

	private:

		int m_back;
		TValue m_data[ MaxSize ];
	};
}
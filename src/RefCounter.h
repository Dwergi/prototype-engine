//
// RefCounter.h - A helper object to hold ref counts for objects referenced by script.
// Copyright (C) Sebastian Nordgren 
// October 11th 2015
//

namespace dd
{
	class RefCounter
	{
	public:

		void Init( void* ptr, const TypeInfo* type )
		{
			m_ptr = ptr;
			m_type = type;
			m_refCount = 1;
		}

		void Increment()
		{
			ASSERT( m_ptr != nullptr );

			++m_refCount;
		}

		void Decrement()
		{
			ASSERT( m_ptr != nullptr );

			--m_refCount;

			if( m_refCount == 0 )
				delete m_ptr;
		}

	private:

		int m_refCount;
		void* m_ptr;
		const TypeInfo* m_type;
	};
}
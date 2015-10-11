//
// RefCounter.h - A helper object to hold ref counts for objects referenced by script.
// Copyright (C) Sebastian Nordgren 
// October 11th 2015
//

namespace dd
{
	template <typename T>
	class RefCounter
	{
	public:

		void Init( T* ptr )
		{
			m_ptr = ptr;
			m_refCount = 1;
		}

		void Increment()
		{
			++m_refCount;
		}

		void Decrement()
		{
			--m_refCount;

			if( m_refCount == 0 )
				delete m_ptr;
		}

	private:

		int m_refCount;
		T* m_ptr;
	};
}
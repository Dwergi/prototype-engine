#pragma once

template<typename TValue, typename TKey, typename TContainer>
class Handle
{
private:

	typedef Handle<TValue, TKey, TContainer> TThis;

	TContainer* m_container;
	TKey m_key;

public:

	Handle()
		: m_container( nullptr )
	{
	}

	Handle( const TThis& other )
		: m_key( other.m_key ),
		m_container( other.m_container )
	{
	}

	Handle( TKey key, TContainer* container ) 
		: m_key( key ), m_container( container )
	{

	}

	~Handle()
	{
		m_container = nullptr;
	}

	TThis& operator=( const TThis& other )
	{
		m_container = other.m_container;
		m_key = other.m_key;

		return *this;
	}

	TValue* operator->()
	{
		assert( m_container != nullptr );

		return &(*m_container)[ m_key ];
	}

	TValue& operator*()
	{
		assert( m_container != nullptr );

		return (*m_container)[ m_key ];
	}

	const TValue* operator->() const
	{
		assert( m_container != nullptr );

		return &(*m_container)[ m_key ];
	}

	const TValue& operator*() const
	{
		assert( m_container != nullptr );

		return (*m_container)[ m_key ];
	}

	bool operator==( const TThis& other ) const
	{
		assert( m_container != nullptr );

		return m_key == other.m_key;
	}

	bool IsValid() const
	{
		return m_container != nullptr;
	}

	void Invalidate() 
	{
		m_container = nullptr;
	}
};
//
// ComponentPairIterator.h - Iterator for vectors of pairs of ints and components.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

template<typename TIter, typename TKey, typename TValue>
class ComponentPairIterator
	: public std::iterator<std::forward_iterator_tag, TValue*>
{
private:

	TIter m_current;

	typedef ComponentPairIterator<TIter, TKey, TValue> ThisType;

public:

	ComponentPairIterator( TIter init )
	{
		m_current = init;
	}

	ComponentPairIterator( const ThisType& other )
	{
		m_current = other.m_current;
	}

	TValue& operator*()
	{
		return m_current->second;
	}

	bool operator==( const ThisType other )
	{
		return m_current == other.m_current;
	}

	bool operator!=( const ThisType other )
	{
		return !operator==( other );
	}

	ThisType& operator++()
	{
		m_current++;

		return *this;
	}
};

template<typename TIter, typename TKey, typename TValue>
class ComponentPairConstIterator
	: public std::iterator<std::forward_iterator_tag, TValue*>
{
private:

	TIter m_current;

	typedef ComponentPairConstIterator<TIter, TKey, TValue> ThisType;

public:

	ComponentPairConstIterator( TIter init )
	{
		m_current = init;
	}

	ComponentPairConstIterator( const ThisType& other )
	{
		m_current = other.m_current;
	}

	const TValue& operator*() const
	{
		return m_current->second;
	}

	bool operator==( const ThisType other )
	{
		return m_current == other.m_current;
	}

	bool operator!=( const ThisType other )
	{
		return !operator==( other );
	}

	ThisType& operator++()
	{
		m_current++;

		return *this;
	}
};


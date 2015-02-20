#pragma once

//
// Pair iterator.
// 
template<typename TKey, typename TValue>
class vector_pair_iterator
	: public std::iterator<std::forward_iterator_tag, TValue*>
{
private:

	typedef typename std::vector<std::pair<typename TKey, typename TValue>>::iterator std_pair_iter;

	typename vector_pair_iterator<TKey, TValue>::std_pair_iter m_current;

public:

	vector_pair_iterator( std_pair_iter& init )
	{
		m_current = init;
	}

	vector_pair_iterator( const vector_pair_iterator<TKey, TValue>& other )
	{
		m_current = other.m_current;
	}

	TValue* operator&()
	{
		return &m_current->second;
	}

	TValue& operator*() 
	{ 
		return m_current->second;
	}

	bool operator==( const vector_pair_iterator<TKey, TValue> other )
	{
		return m_current == other.m_current;
	}

	bool operator!=( const vector_pair_iterator<TKey, TValue> other )
	{
		return !operator==( other );
	}

	vector_pair_iterator<TKey, TValue>& operator++()
	{
		m_current++;

		return *this;
	}
};

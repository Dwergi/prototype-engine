#pragma once

#include "Property.h"
//---------------------------------------------------------------------------

class RecorderBase
{
};

template< typename T >
class Recorder
	: public RecorderBase
{
protected:

	static const int HISTORY_SIZE = 32;

	T* m_current;
	std::vector<T> m_undo;

public:

	Recorder( Property* prop )
		: m_current( prop->GetPtr<T>() )
	{
	}

	Recorder( T& value, int size = HISTORY_SIZE )
		: m_current( &value )
	{
		m_undo.reserve( HISTORY_SIZE );
	}

	Recorder<T>& operator=( const T& newValue )
	{
		if( m_undo.size() == HISTORY_SIZE )
		{
			m_undo.erase( m_undo.begin() );
		}

		m_undo.push_back( *m_current );

		*m_current = newValue;

		return *this;
	}

	void Undo()
	{
		ASSERT( m_undo.size() > 0 );

		*m_current = m_undo.back();
		m_undo.pop_back();
	}

	operator T()
	{
		return *m_current;
	}

	operator T() const
	{
		return *m_current;
	}

	T& Get()
	{
		return *m_current;
	}

	const T& Get() const
	{
		return *m_current;
	}

	bool operator==( const T& other ) const
	{
		return *m_current == other;
	}

	int GetUndoHistorySize() const
	{
		return m_undo.size();
	}
};
//---------------------------------------------------------------------------

template< typename T >
class FullRecorder
	: public Recorder<T> 
{
protected:

	std::vector<T> m_redo;

	typedef Recorder<T> base;

public:

	FullRecorder( T& value, int history_size = Recorder<T>::HISTORY_SIZE )
		: base( value, history_size )
	{
		m_redo.reserve( history_size );
	}

	FullRecorder<T>& operator=( const T& newValue )
	{
		m_redo.clear();

		base::operator=( newValue );

		return *this;
	}

	void Undo()
	{
		if( m_redo.size() == HISTORY_SIZE )
		{
			m_redo.erase( m_redo.begin() );
		}

		m_redo.push_back( *m_current );

		base::Undo();
	}

	void Redo()
	{
		ASSERT( m_redo.size() > 0 );

		base::operator=( m_redo.back() );
		m_redo.pop_back();
	}

	int GetRedoHistorySize() const
	{
		return m_redo.size();
	}
};
//---------------------------------------------------------------------------

namespace tests
{
	static void RecorderTests() 
	{
		int ITERATION_COUNT = 99;
		int HISTORY_SIZE = 32;

		int value = -1;
		std::vector<int> values;
		FullRecorder<int> test( value, HISTORY_SIZE );
		values.push_back( test );

		for( int i = 0; i < ITERATION_COUNT; ++i )
		{
			test = i;
			values.push_back( test );
		}

		int undo_count = test.GetUndoHistorySize();
		ASSERT( undo_count == HISTORY_SIZE );

		for( int i = ITERATION_COUNT - 1; i >= ITERATION_COUNT - undo_count; --i )
		{
			test.Undo();

			ASSERT( test == values[ i ] );
			ASSERT( test.GetUndoHistorySize() >= 0 );
		}

		ASSERT( test.GetUndoHistorySize() == 0 );

		std::cout << "[Recorder] Successfully tested undo for " << ITERATION_COUNT << " values with history size " << HISTORY_SIZE << "." << std::endl;
	}

	static void FullRecorderTests()
	{
		int ITERATION_COUNT = 64;
		int HISTORY_SIZE = 32;

		int value = -1;
		std::vector<int> values;
		FullRecorder<int> test( value, HISTORY_SIZE );
		values.push_back( test );

		for( int i = 0; i < ITERATION_COUNT; ++i )
		{
			test = i;
			values.push_back( test );
		}

		int undo_count = test.GetUndoHistorySize();
		ASSERT( undo_count == HISTORY_SIZE );

		for( int i = ITERATION_COUNT - 1; i >= ITERATION_COUNT - undo_count; --i )
		{
			test.Undo();

			ASSERT( test == values[ i ] );
			ASSERT( test.GetUndoHistorySize() >= 0 );
		}

		ASSERT( test.GetUndoHistorySize() == 0 );

		int redo_count = test.GetRedoHistorySize();
		ASSERT( redo_count == HISTORY_SIZE );

		for( int i = ITERATION_COUNT - redo_count; i < ITERATION_COUNT; ++i )
		{
			test.Redo();

			ASSERT( test == values[ i + 1 ] );
			ASSERT( test.GetRedoHistorySize() >= 0 );
		}

		ASSERT( test.GetRedoHistorySize() == 0 );
		ASSERT( test.GetUndoHistorySize() == HISTORY_SIZE );

		std::cout << "[FullRecorder] Successfully tested undo and redo for " << ITERATION_COUNT << " values with history size " << HISTORY_SIZE << "." << std::endl;
	}
}
//===========================================================================

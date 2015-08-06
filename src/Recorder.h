//
// Recorder.h - A wrapper class that transparently records the history of a certain property.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#include "Property.h"

namespace dd
{
	class RecorderBase
	{
	};
	//---------------------------------------------------------------------------

	template< typename T >
	class Recorder
		: public RecorderBase
	{
	protected:

		static const int HISTORY_SIZE = 32;

		T* m_current;
		std::vector<T> m_undo;

	public:

		Recorder( dd::Property* prop )
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

		uint GetUndoHistorySize() const
		{
			return (uint) m_undo.size();
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

		uint GetRedoHistorySize() const
		{
			return (uint) m_redo.size();
		}
	};
}

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
		Vector<T> m_undo;

	public:

		Recorder( dd::Property* prop )
			: m_current( (T*) prop->GetPtr() )
		{
		}

		Recorder( T& value, int size = HISTORY_SIZE )
			: m_current( &value )
		{
			m_undo.Resize( HISTORY_SIZE );
		}

		Recorder<T>& operator=( const T& newValue )
		{
			if( m_undo.Size() == HISTORY_SIZE )
			{
				m_undo.RemoveOrdered( 0 );
			}

			m_undo.Add( *m_current );

			*m_current = newValue;

			return *this;
		}

		void Undo()
		{
			ASSERT( m_undo.Size() > 0 );

			*m_current = m_undo.Pop();
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
			return (uint) m_undo.Size();
		}
	};
	//---------------------------------------------------------------------------

	template< typename T >
	class FullRecorder
		: public Recorder<T> 
	{
	protected:

		Vector<T> m_redo;

		typedef Recorder<T> base;

	public:

		FullRecorder( T& value, int history_size = Recorder<T>::HISTORY_SIZE )
			: base( value, history_size )
		{
			m_redo.Resize( history_size );
		}

		FullRecorder<T>& operator=( const T& newValue )
		{
			m_redo.Clear();

			base::operator=( newValue );

			return *this;
		}

		void Undo()
		{
			if( m_redo.Size() == HISTORY_SIZE )
			{
				m_redo.RemoveOrdered( 0 );
			}

			m_redo.Add( *m_current );

			base::Undo();
		}

		void Redo()
		{
			ASSERT( m_redo.Size() > 0 );

			base::operator=( m_redo.Pop() );
		}

		uint GetRedoHistorySize() const
		{
			return (uint) m_redo.Size();
		}
	};
}

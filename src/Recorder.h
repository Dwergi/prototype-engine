//
// Recorder.h - A wrapper class that transparently records the history of a certain property.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#include "Property.h"

namespace dd
{
	struct RecorderBase
	{
	};
	//---------------------------------------------------------------------------

	template <typename T>
	struct Recorder
		: RecorderBase
	{
	protected:

		static const int HISTORY_SIZE = 32;

		T* m_current;
		Vector<T> m_undo;

	public:

		explicit Recorder( dd::Property* prop );
		explicit Recorder( T& value, int size = HISTORY_SIZE );
		
		Recorder<T>& operator=( const T& newValue );

		void Undo();

		operator T();
		operator T() const;

		T& Get();
		const T& Get() const;

		bool operator==( const T& other ) const;

		uint GetUndoHistorySize() const;
	};
	//---------------------------------------------------------------------------

	template <typename T>
	struct FullRecorder
		: Recorder<T> 
	{
	protected:

		Vector<T> m_redo;
		typedef Recorder<T> base;

	public:

		explicit FullRecorder( T& value, int history_size = Recorder<T>::HISTORY_SIZE );

		FullRecorder<T>& operator=( const T& newValue );

		void Undo();
		void Redo();
		uint GetRedoHistorySize() const;
	};
}

#include "Recorder.inl"

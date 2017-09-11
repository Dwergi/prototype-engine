namespace dd
{
	template <typename T>
	Recorder<T>::Recorder( Property* prop )
		: m_current( (T*) prop->GetPtr() )
	{
	}

	template <typename T>
	Recorder<T>::Recorder( T& value, int size )
		: m_current( &value )
	{
		m_undo.Reserve( HISTORY_SIZE );
	}

	template <typename T>
	Recorder<T>& Recorder<T>::operator=( const T& newValue )
	{
		if( m_undo.Size() == HISTORY_SIZE )
		{
			m_undo.RemoveOrdered( 0 );
		}

		m_undo.Add( *m_current );

		*m_current = newValue;

		return *this;
	}

	template <typename T>
	void Recorder<T>::Undo()
	{
		DD_ASSERT( m_undo.Size() > 0 );

		*m_current = m_undo.Pop();
	}

	template <typename T>
	Recorder<T>::operator T()
	{
		return *m_current;
	}

	template <typename T>
	Recorder<T>::operator T() const
	{
		return *m_current;
	}

	template <typename T>
	T& Recorder<T>::Get()
	{
		return *m_current;
	}

	template <typename T>
	const T& Recorder<T>::Get() const
	{
		return *m_current;
	}

	template <typename T>
	bool Recorder<T>::operator==( const T& other ) const
	{
		return *m_current == other;
	}

	template <typename T>
	uint Recorder<T>::GetUndoHistorySize() const
	{
		return m_undo.Size();
	}
	//---------------------------------------------------------------------------

	template <typename T>
	FullRecorder<T>::FullRecorder( T& value, int history_size )
		: base( value, history_size )
	{
		m_redo.Reserve( history_size );
	}

	template <typename T>
	FullRecorder<T>& FullRecorder<T>::operator=( const T& newValue )
	{
		m_redo.Clear();

		base::operator=( newValue );

		return *this;
	}

	template <typename T>
	void FullRecorder<T>::Undo()
	{
		if( m_redo.Size() == HISTORY_SIZE )
		{
			m_redo.RemoveOrdered( 0 );
		}

		m_redo.Add( *m_current );

		base::Undo();
	}

	template <typename T>
	void FullRecorder<T>::Redo()
	{
		DD_ASSERT( m_redo.Size() > 0 );

		base::operator=( m_redo.Pop() );
	}

	template <typename T>
	uint FullRecorder<T>::GetRedoHistorySize() const
	{
		return m_redo.Size();
	}
}
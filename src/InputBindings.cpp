//
// InputBindings.h - Handler for binding actions to functions.
// Dispatches actions to registered functions.
// Copyright (C) Sebastian Nordgren 
// April 12th 2016
//

#include "PCH.h"
#include "InputBindings.h"

namespace dd
{
	InputBindings::InputBindings()
	{
	}

	InputBindings::~InputBindings()
	{
	}

	void InputBindings::RegisterHandler( InputAction action, InputBindings::InputHandler handler )
	{
		Vector<InputHandler>* handlers = m_bindings.Find( action );

		if( handlers == nullptr )
		{
			m_bindings.Add( action, Vector<InputHandler>() );
			handlers = m_bindings.Find( action );
		}

		handlers->Add( handler );
	}

	void InputBindings::Dispatch( const IArray<InputEvent>& actions ) const
	{
		for( const InputEvent& evt : actions )
		{
			Vector<InputHandler>* handlers = m_bindings.Find( evt.Action );
			if( handlers == nullptr )
				continue;

			for( InputHandler& handler : *handlers )
			{
				handler( evt.Action, evt.Type );
			}
		}
	}
}
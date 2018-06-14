//
// InputBindings.h - Handler for binding actions to functions.
// Dispatches actions to registered functions.
// Copyright (C) Sebastian Nordgren 
// April 12th 2016
//

#pragma once

#include "Input.h"

#include <functional>

namespace dd
{
	class InputBindings
	{
	public:

		typedef std::function<void( InputAction, InputType )> InputHandler;
		
		InputBindings();
		~InputBindings();

		void RegisterHandler( InputAction action, InputHandler handler );
		void Dispatch( const IArray<InputEvent>& actions ) const;

	private:
		
		DenseMap<InputAction, Vector<InputHandler>> m_bindings;
	};
}

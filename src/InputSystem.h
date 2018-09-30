//
// InputSystem.h
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "System.h"

namespace dd
{
	struct IInputSource;
	struct InputBindings;

	struct InputSystem
	{
		InputSystem( IInputSource& source, InputBindings& bindings );

		void Update( float delta_t );

		void BindKeys();

		IInputSource& Source() { return m_source; }
		InputBindings& Bindings() { return m_bindings; }

	private:

		IInputSource& m_source;
		InputBindings& m_bindings;
	};
}
//
// InputSystem.h
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "System.h"

namespace dd
{
	struct InputBindings;
	struct IInputSource;

	struct InputSystem
	{
		InputSystem();

		void Update( float delta_t );
		void BindKeys();

		void SetBindings(const InputBindings& bindings);
		InputBindings* GetBindings() const { return m_bindings; }
		IInputSource& GetSource() const;

	private:
		InputBindings* m_bindings;
	};
}
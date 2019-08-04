//
// InputSystem.h
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#pragma once

#include "InputMode.h"
#include "System.h"

namespace dd
{
	struct InputBindings;
	struct IInputSource;

	struct InputSystem
	{
		InputSystem();

		void Initialize();

		void Update( float delta_t );

		void SetBindings(const InputBindings& bindings) { m_bindings = &bindings; }
		const InputBindings* GetBindings() const { return m_bindings; }
		IInputSource& GetSource() const;

		void SetMode(int16 id);
		int16 GetMode() { return m_currentMode; }

	private:
		const InputBindings* m_bindings { nullptr };
		int16 m_currentMode { InputMode::NONE };
		int16 m_nextMode { InputMode::NONE };

		void BindKeys();
	};
}
//
// ScriptComponent.h - Base class for a scriptable component.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "IComponent.h"
#include "DenseMapPool.h"

namespace dd
{
	class SharedString;

	class ScriptComponent : public IComponent
	{
	public:
		using Pool = DenseMapPool<ScriptComponent>;

		ScriptComponent();

		void SetModule( const String& moduleName ) { m_module = moduleName; }
		const String& GetModule() const { return m_module; }

		bool IsLoaded() const { return m_loaded; }
		void SetLoaded( bool loaded ) { m_loaded = loaded; }

		BEGIN_TYPE( ScriptComponent )
		END_TYPE

	private:
		String64 m_module;
		bool m_loaded { false };
	};
}

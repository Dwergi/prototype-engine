//
// ScriptSystem.h - System that allows ScriptComponents to update every frame.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "System.h"

namespace ddc
{
	struct UpdateData;
}

namespace dd
{
	class ScriptSystem : ddc::System
	{
	public:
		ScriptSystem( AngelScriptEngine& script_engine );
		~ScriptSystem();

		virtual void Initialize( ddc::World& ) {}
		virtual void Update( const ddc::UpdateData& data, float dt );
		virtual void Shutdown( ddc::World& ) {}

	private:
		AngelScriptEngine& m_scriptEngine;
	};
}


//
// ScriptSystem.h - System that allows ScriptComponents to update every frame.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

#include "ISystem.h"

namespace dd
{
	class ScriptSystem : ISystem
	{
	public:
		ScriptSystem( AngelScriptEngine& script_engine );
		~ScriptSystem();

		virtual void Update( EntityManager& entity_manager, float dt ) override;

	private:

		AngelScriptEngine& m_scriptEngine;
	};
}


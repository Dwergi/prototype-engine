//
// ScriptComponent.h - Base class for a scriptable component.
// Copyright (C) Sebastian Nordgren 
// October 29th 2017
//

#pragma once

namespace dd
{
	class SharedString;

	struct ScriptComponent
	{
		String64 Module;
		bool Loaded { false };

		DD_CLASS( dd::ScriptComponent )
		{
			DD_COMPONENT();

			DD_MEMBER( dd::ScriptComponent, Module );
			DD_MEMBER( dd::ScriptComponent, Loaded );
		}
	};
}

//
// JSON.h - Wrapper around JSON parsing for objects with TypeInfo.
// Copyright (C) Sebastian Nordgren 
// October 8th 2018
//

#pragma once

#include <nlohmann/json_fwd.hpp>

namespace dd
{
	struct Variable;

	struct JSON
	{
		static void Read( Variable& var, const nlohmann::json& in );
		static void Write( Variable& var, nlohmann::json& out );
	};
}
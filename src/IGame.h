//
// IGame.h - Interface for games.
// Copyright (C) Sebastian Nordgren 
// August 2nd 2019
//

#pragma once

#include "Input.h"

namespace dd
{
	struct GameUpdateData
	{
		GameUpdateData(ddc::EntitySpace& space, Input& input, float delta_t) : 
			m_space(space), m_input(input), m_delta(delta_t) {}

		// Get the entity space.
		ddc::EntitySpace& Space() const { return m_space; }
		
		// Get the delta since last frame.
		float Delta() const { return m_delta; }

	private:
		ddc::EntitySpace& m_space;
		float m_delta;
	};

	struct IGame
	{
		virtual void Initialize(ddc::EntitySpace& entities) = 0;
		virtual void Shutdown(ddc::EntitySpace& entities) = 0;
		virtual void Update(dd::GameUpdateData& entities) = 0;
		virtual void RenderUpdate(dd::GameUpdateData& entities) = 0;

		//
		// Fetch the entity spaces for the game.
		//
		virtual const std::vector<ddc::EntitySpace>& GetEntitySpaces() = 0;

		virtual const char* GetTitle() const { return "DD"; }
	};
}
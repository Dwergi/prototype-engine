//
// SFMLInputSource.h
// Copyright (C) Sebastian Nordgren 
// August 2nd 2018
//

#pragma once

#include "IInputSource.h"

namespace sf
{
	class Window;
}

namespace dd
{
	struct SFMLInputSource : dd::IInputSource
	{
		SFMLInputSource();

	private:
		virtual void OnUpdateInput() override;
		virtual void OnSetMouseCapture(bool capture) override;
		virtual void OnSetMousePosition(glm::vec2 pos) override;

		sf::Window* m_sfmlWindow { nullptr };
	};
}
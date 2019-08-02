//
// Input.cpp - Wrappers around GLFW input functions.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#include "PCH.h"
#include "SFMLInputSource.h"

#include "IWindow.h"

#include "sfml/Window.hpp"

dd::Service<dd::IWindow> s_window;

namespace dd
{
	SFMLInputSource::SFMLInputSource()
	{
		m_sfmlWindow = (sf::Window*) s_window->GetNative();

		DD_ASSERT(m_sfmlWindow != nullptr, "Window not initialized!");
	}

	SFMLInputSource::~SFMLInputSource()
	{
	}

	static int GetKeyCode(dd::IInputSource& src, sf::Keyboard::Key key)
	{
		int key = -1;
		switch (key_evt.code)
		{
			case sf::Keyboard::A: key = 'a'; break;
			case sf::Keyboard::B: key = 'b'; break;
			case sf::Keyboard::C: key = 'c'; break;
			case sf::Keyboard::D: key = 'd'; break;
			case sf::Keyboard::E: key = 'e'; break;
			case sf::Keyboard::F: key = 'f'; break;
			case sf::Keyboard::G: key = 'g'; break;
			case sf::Keyboard::H: key = 'h'; break;
			case sf::Keyboard::I: key = 'i'; break;
			case sf::Keyboard::J: key = 'j'; break;
			case sf::Keyboard::K: key = 'k'; break;
			case sf::Keyboard::L: key = 'l'; break;
			case sf::Keyboard::M: key = 'm'; break;
			case sf::Keyboard::N: key = 'n'; break;
			case sf::Keyboard::O: key = 'o'; break;
			case sf::Keyboard::P: key = 'p'; break;
			case sf::Keyboard::Q: key = 'q'; break;
			case sf::Keyboard::R: key = 'r'; break;
			case sf::Keyboard::S: key = 's'; break;
			case sf::Keyboard::T: key = 't'; break;
			case sf::Keyboard::U: key = 'u'; break;
			case sf::Keyboard::V: key = 'v'; break;
			case sf::Keyboard::W: key = 'w'; break;
			case sf::Keyboard::X: key = 'x'; break;
			case sf::Keyboard::Y: key = 'y'; break;
			case sf::Keyboard::Z: key = 'z'; break;
			case sf::Keyboard::Escape: key = ( int) Key::ESCAPE; break;
			case sf::Keyboard::Enter: key = ( int) Key::ENTER; break;
			case sf::Keyboard::Tab: key = ( int) Key::TAB; break;
			case sf::Keyboard::PageUp: key = ( int) Key::PAGE_UP; break;
			case sf::Keyboard::PageDown: key = ( int) Key::PAGE_DOWN; break;
			case sf::Keyboard::Home: key = ( int) Key::HOME; break;
			case sf::Keyboard::End: key = ( int) Key::END; break;
			case sf::Keyboard::Pause: key = ( int) Key::PAUSE; break;
			case sf::Keyboard::F1: key = ( int) Key::F1; break;
			case sf::Keyboard::F2: key = ( int) Key::F2; break;
			case sf::Keyboard::F3: key = ( int) Key::F3; break;
			case sf::Keyboard::F4: key = ( int) Key::F4; break;
			case sf::Keyboard::F5: key = ( int) Key::F5; break;
			case sf::Keyboard::F6: key = ( int) Key::F6; break;
			case sf::Keyboard::F7: key = ( int) Key::F7; break;
			case sf::Keyboard::F8: key = ( int) Key::F8; break;
			case sf::Keyboard::F9: key = ( int) Key::F9; break;
			case sf::Keyboard::F10: key = ( int) Key::F10; break;
			case sf::Keyboard::LShift: key = ( int) Key::LSHIFT; break;
			case sf::Keyboard::LControl: key = ( int) Key::LCTRL; break;
			case sf::Keyboard::LAlt: key = ( int) Key::LALT; break;
		}

	}
	
	void SFMLInputSource::OnUpdateInput()
	{
		sf::Event evt;
		while (m_sfmlWindow->pollEvent(evt))
		{
			switch (evt.type)
			{
				case sf::Event::KeyPressed:
					AddKeyEvent(evt.key, InputType::PRESSED);
					break;

				case sf::Event::KeyReleased:
					AddKeyEvent(evt.key, InputType::RELEASED);
					break;

				case sf::Event::MouseButtonPressed:
					AddMouseButtonEvent(evt.mouseButton, InputType::PRESSED);
					break;

				case sf::Event::MouseButtonReleased:
					AddMouseButtonEvent(evt.mouseButton, InputType::RELEASED);
					break;

				case sf::Event::MouseMoved:
					OnMousePosition(glm::vec2(evt.mouseMove.x, evt.mouseMove.y));
					break;

				case sf::Event::MouseWheelScrolled:
					OnMouseWheel(glm::vec2(evt.mouseWheelScroll.x, evt.mouseWheelScroll.y));
					break;

				case sf::Event::TextEntered:
					OnTextEvent(evt.text.unicode);
					break;
			}
		}
	}

	void SFMLInputSource::OnCaptureMouse(bool capture)
	{
		m_sfmlWindow->setMouseCursorGrabbed(capture);
	}
}
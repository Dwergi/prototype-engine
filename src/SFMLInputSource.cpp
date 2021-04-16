//
// SFMLInputSource.cpp
// Copyright (C) Sebastian Nordgren 
// August 2nd 2018
//

#include "PCH.h"
#include "SFMLInputSource.h"

#include "IWindow.h"

#include <sfml/Window.hpp>

static dd::Service<dd::IWindow> s_window;

namespace dd
{
	SFMLInputSource::SFMLInputSource()
	{
		m_sfmlWindow = (sf::Window*) s_window->GetNative();

		DD_ASSERT(m_sfmlWindow != nullptr, "Window not initialized!");
	}

	static Key GetKeyCode(sf::Keyboard::Key sfml_key)
	{
		switch (sfml_key)
		{
			case sf::Keyboard::A: return Key::A;
			case sf::Keyboard::B: return Key::B;
			case sf::Keyboard::C: return Key::C;
			case sf::Keyboard::D: return Key::D;
			case sf::Keyboard::E: return Key::E;
			case sf::Keyboard::F: return Key::F;
			case sf::Keyboard::G: return Key::G;
			case sf::Keyboard::H: return Key::H;
			case sf::Keyboard::I: return Key::I;
			case sf::Keyboard::J: return Key::J;
			case sf::Keyboard::K: return Key::K;
			case sf::Keyboard::L: return Key::L;
			case sf::Keyboard::M: return Key::M;
			case sf::Keyboard::N: return Key::N;
			case sf::Keyboard::O: return Key::O;
			case sf::Keyboard::P: return Key::P;
			case sf::Keyboard::Q: return Key::Q;
			case sf::Keyboard::R: return Key::R;
			case sf::Keyboard::S: return Key::S;
			case sf::Keyboard::T: return Key::T;
			case sf::Keyboard::U: return Key::U;
			case sf::Keyboard::V: return Key::V;
			case sf::Keyboard::W: return Key::W;
			case sf::Keyboard::X: return Key::X;
			case sf::Keyboard::Y: return Key::Y;
			case sf::Keyboard::Z: return Key::Z;
			case sf::Keyboard::LShift: return Key::LSHIFT;
			case sf::Keyboard::LControl: return Key::LCTRL;
			case sf::Keyboard::LAlt: return Key::LALT;
			case sf::Keyboard::Hyphen: return Key::HYPHEN;
			case sf::Keyboard::Period: return Key::PERIOD;
			case sf::Keyboard::Escape: return Key::ESCAPE;
			case sf::Keyboard::Space: return Key::SPACE;
			case sf::Keyboard::Enter: return Key::ENTER;
			case sf::Keyboard::Backspace: return Key::BACKSPACE;
			case sf::Keyboard::Tab: return Key::TAB;
			case sf::Keyboard::PageUp: return Key::PAGE_UP;
			case sf::Keyboard::PageDown: return Key::PAGE_DOWN;
			case sf::Keyboard::Home: return Key::HOME;
			case sf::Keyboard::End: return Key::END;
			case sf::Keyboard::Delete: return Key::DEL;
			case sf::Keyboard::Pause: return Key::PAUSE;
			case sf::Keyboard::Left: return Key::LEFT_ARROW;
			case sf::Keyboard::Right: return Key::RIGHT_ARROW;
			case sf::Keyboard::Up: return Key::UP_ARROW;
			case sf::Keyboard::Down: return Key::DOWN_ARROW;
			case sf::Keyboard::F1: return Key::F1;
			case sf::Keyboard::F2: return Key::F2;
			case sf::Keyboard::F3: return Key::F3;
			case sf::Keyboard::F4: return Key::F4;
			case sf::Keyboard::F5: return Key::F5;
			case sf::Keyboard::F6: return Key::F6;
			case sf::Keyboard::F7: return Key::F7;
			case sf::Keyboard::F8: return Key::F8;
			case sf::Keyboard::F9: return Key::F9;
			case sf::Keyboard::F10: return Key::F10;

			default: return Key::NONE;
		}
	}

	static ModifierFlags GetModifiers( const sf::Event::KeyEvent& evt )
	{
		ModifierFlags modifiers;
		if( evt.alt )
		{
			modifiers.Set(Modifier::Alt);
		}

		if( evt.control )
		{
			modifiers.Set(Modifier::Ctrl);
		}

		if( evt.shift )
		{
			modifiers.Set(Modifier::Shift);
		}

		return modifiers;
	}

	static Key GetMouseButton(const sf::Event::MouseButtonEvent& evt)
	{
		switch (evt.button)
		{
		case sf::Mouse::Left:
			return Key::MOUSE_LEFT;

		case sf::Mouse::Right:
			return Key::MOUSE_RIGHT;

		case sf::Mouse::Middle:
			return Key::MOUSE_MIDDLE; 

		case sf::Mouse::XButton1:
			return Key::MOUSE_4;

		case sf::Mouse::XButton2:
			return Key::MOUSE_5;
		}

		return Key::NONE;
	}
	
	void SFMLInputSource::OnUpdateInput()
	{
		sf::Event evt;
		while (m_sfmlWindow->pollEvent(evt))
		{
			switch (evt.type)
			{
				case sf::Event::KeyPressed:
				{
					Key key = GetKeyCode( evt.key.code );
					ModifierFlags modifiers = GetModifiers( evt.key );
					OnKey( key, modifiers, InputType::Press );
					break;
				}
				case sf::Event::KeyReleased:
				{
					Key key = GetKeyCode(evt.key.code);
					ModifierFlags modifiers = GetModifiers(evt.key);
					OnKey(key, modifiers, InputType::Release);
					break;
				}
				case sf::Event::MouseButtonPressed:
					OnKey(GetMouseButton(evt.mouseButton), Modifier::None, InputType::Press);
					break;

				case sf::Event::MouseButtonReleased:
					OnKey(GetMouseButton(evt.mouseButton), Modifier::None, InputType::Release);
					break;

				case sf::Event::MouseMoved:
					OnMousePosition(glm::vec2(evt.mouseMove.x, evt.mouseMove.y));
					break;

				case sf::Event::MouseWheelScrolled:
				{
					glm::vec2 delta;
					delta.x = evt.mouseWheelScroll.wheel == sf::Mouse::Wheel::HorizontalWheel ? evt.mouseWheelScroll.delta : 0;
					delta.y = evt.mouseWheelScroll.wheel == sf::Mouse::Wheel::VerticalWheel ? evt.mouseWheelScroll.delta : 0;
					OnMouseWheel(delta);
					break;
				}

				case sf::Event::TextEntered:
					OnText(evt.text.unicode);
					break;

				case sf::Event::Closed:
					s_window->SetToClose();
					break;

				case sf::Event::Resized:
					s_window->SetSize(glm::ivec2(evt.size.width, evt.size.height));
					break;
			}
		}
	}

	void SFMLInputSource::OnSetMousePosition(glm::vec2 pos)
	{
		sf::Mouse::setPosition(sf::Vector2i((int) pos.x, (int) pos.y), *m_sfmlWindow);
	}

	void SFMLInputSource::OnSetMouseCapture(bool capture)
	{
		m_sfmlWindow->setMouseCursorGrabbed(capture);
	}
}
//
// InputSystem.cpp
// Copyright (C) Sebastian Nordgren 
// September 27th 2018
//

#include "PCH.h"
#include "InputSystem.h"

#include "IInputSource.h"
#include "InputBindings.h"

#include <imgui/imgui.h>

namespace dd
{
	InputSystem::InputSystem( IInputSource& source, InputBindings& bindings ) :
		m_source( source ),
		m_bindings( bindings )
	{
	}

	void InputSystem::Update( float delta_t )
	{
		m_source.UpdateInput();

		// update input
		if( !ImGui::GetIO().WantCaptureKeyboard )
		{
			Array<InputEvent, 64> events;
			m_source.GetKeyEvents( events );
			m_bindings.Dispatch( events );
		}

		if( !ImGui::GetIO().WantCaptureMouse )
		{
			Array<InputEvent, 64> events;
			m_source.GetMouseEvents( events );
			m_bindings.Dispatch( events );
		}
	}

	void InputSystem::BindKeys()
	{
		m_source.BindKey( Key::F1, InputAction::TOGGLE_FREECAM );
		m_source.BindKey( Key::F2, InputAction::TOGGLE_CONSOLE );
		m_source.BindKey( Key::ESCAPE, InputAction::TOGGLE_DEBUG_UI );
		m_source.BindKey( 'W', InputAction::FORWARD );
		m_source.BindKey( 'S', InputAction::BACKWARD );
		m_source.BindKey( 'A', InputAction::LEFT );
		m_source.BindKey( 'D', InputAction::RIGHT );
		m_source.BindKey( ' ', InputAction::UP );
		m_source.BindKey( 'R', InputAction::ADD_MINOR_TRAUMA );
		m_source.BindKey( 'T', InputAction::ADD_MAJOR_TRAUMA );
		//m_source.BindKey( 'P', InputAction::TOGGLE_PICKING );
		m_source.BindKey( Key::LCTRL, InputAction::DOWN );
		m_source.BindKey( Key::LSHIFT, InputAction::BOOST );
		//m_source.BindMouseButton( MouseButton::LEFT, InputAction::SELECT_MESH );
		m_source.BindMouseButton( MouseButton::LEFT, InputAction::SHOOT );
		m_source.BindKey( Key::PAUSE, InputAction::BREAK );
		m_source.BindKey( 'E', InputAction::START_PARTICLE );
		m_source.BindKey( 'P', InputAction::RESET_PHYSICS );

		m_source.BindKey( '1', InputAction::CAMERA_POS_1 );
		m_source.BindKey( '2', InputAction::CAMERA_POS_2 );
		m_source.BindKey( '3', InputAction::CAMERA_POS_3 );
		m_source.BindKey( '4', InputAction::CAMERA_POS_4 );

		m_source.BindKey( Key::HOME, InputAction::DECREASE_DEPTH );
		m_source.BindKey( Key::END, InputAction::INCREASE_DEPTH );
	}
}

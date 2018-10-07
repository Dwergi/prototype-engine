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
		m_source.BindKey( InputAction::TOGGLE_DEBUG_UI,		Key::ESCAPE	);
		m_source.BindKey( InputAction::TOGGLE_FREECAM,		Key::F1		);
		m_source.BindKey( InputAction::TOGGLE_BOUNDS,		Key::F2		);
		m_source.BindKey( InputAction::TOGGLE_PICKING,		Key::F3,		InputMode::DEBUG );
		m_source.BindKey( InputAction::FORWARD,				'W'			);
		m_source.BindKey( InputAction::BACKWARD,			'S'			);
		m_source.BindKey( InputAction::LEFT,				'A'			);
		m_source.BindKey( InputAction::RIGHT,				'D'			);
		m_source.BindKey( InputAction::UP,					' '			);
		m_source.BindKey( InputAction::ADD_MINOR_TRAUMA,	'R'			);
		m_source.BindKey( InputAction::ADD_MAJOR_TRAUMA,	'T'			);
		m_source.BindKey( InputAction::DOWN,				Key::LCTRL	);
		m_source.BindKey( InputAction::BOOST,				Key::LSHIFT	);
		m_source.BindKey( InputAction::TOGGLE_ENTITY_DATA,	'E',			InputMode::DEBUG );
		m_source.BindKey( InputAction::CAMERA_POS_1,		'1'			);
		m_source.BindKey( InputAction::CAMERA_POS_2,		'2'			);
		m_source.BindKey( InputAction::CAMERA_POS_3,		'3'			);
		m_source.BindKey( InputAction::CAMERA_POS_4,		'4'			);
		m_source.BindKey( InputAction::DECREASE_DEPTH,		Key::HOME	);
		m_source.BindKey( InputAction::INCREASE_DEPTH,		Key::END	);
		m_source.BindKey( InputAction::RESET_PHYSICS,		'P',			InputMode::GAME );
		m_source.BindKey( InputAction::TOGGLE_PICKING,		'P',			InputMode::DEBUG );
		m_source.BindKey( InputAction::PAUSE,				Key::PAUSE	);
		m_source.BindKey( InputAction::TIME_SCALE_DOWN,		Key::PAGE_DOWN );
		m_source.BindKey( InputAction::TIME_SCALE_UP,		Key::PAGE_UP );
																	   
		//m_source.BindKey( InputAction::TOGGLE_CONSOLE, Key::F2, InputMode::DEBUG );

		m_source.BindMouseButton( InputAction::SELECT_MESH,	MouseButton::LEFT,	InputMode::DEBUG );
		m_source.BindMouseButton( InputAction::SHOOT,		MouseButton::LEFT,	InputMode::GAME );
	}
}

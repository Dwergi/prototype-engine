#include "PrecompiledHeader.h"
#include "FSM.h"

namespace dd
{
	FSM::FSM()
	{

	}

	FSM::~FSM()
	{

	}

	void FSM::Initialize( int id )
	{
		auto it = m_states.find( id );
		if( it != m_states.end() )
		{
			m_current = &it->second;
		}
	}

	bool FSM::TransitionTo( int id )
	{
		for( std::pair<int, int>& transition : m_transitions )
		{
			if( transition.first == m_current->ID() && transition.second == id )
			{
				m_current->Exit();
				
				auto it = m_states.find( id );
				DD_ASSERT( it != m_states.end(), "Transition state not found!" );

				m_current = &it->second;
				m_current->Enter();

				return true;
			}
		}

		return false;
	}

	void FSM::AddTransition( int from, int to )
	{
		DD_ASSERT( m_states.find( from ) != m_states.end(), "From state not registered yet!" );
		DD_ASSERT( m_states.find( to ) != m_states.end(), "To state not registered yet!" );

		m_transitions.Add( std::make_pair( from, to ) );
	}

	FSMState& FSM::AddState( int id )
	{
		auto it = m_states.insert( std::make_pair( id, FSMState( id ) ) );
		return it.first->second;
	}

	FSMState::FSMState( int id ) :
		m_id( id )
	{
		
	}

	FSMState::FSMState( const FSMState& other ) :
		m_id( other.m_id )
	{

	}

	FSMState::~FSMState()
	{

	}

	void FSMState::SetOnEnter( FunctionView<void()> on_enter )
	{	
		m_onEnter = on_enter;
	}

	void FSMState::SetOnExit( FunctionView<void()> on_exit )
	{
		m_onExit = on_exit;
	}

	void FSMState::Enter() const
	{
		if( m_onEnter.valid() )
		{
			m_onEnter();
		}
	}

	void FSMState::Exit() const 
	{
		if( m_onExit.valid() )
		{
			m_onExit();
		}
	}
}
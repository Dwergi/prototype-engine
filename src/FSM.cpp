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
		m_id( other.m_id ),
		m_transitions( other.m_transitions )
	{

	}

	FSMState::~FSMState()
	{

	}

	void FSMState::AddTransition( FSMState to, FunctionView<void()> transition )
	{
		auto it = m_transitions.find( to.ID() );
		DD_ASSERT( it == m_transitions.end(), "Adding duplicate state!" );
		
		m_transitions.insert( std::make_pair( to.ID(), transition ) );
	}
}
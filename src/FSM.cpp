#include "PCH.h"
#include "FSM.h"

namespace dd
{
	FSMPrototype::State::State( int id ) :
		m_id( id )
	{

	}

	FSMPrototype::State::State( const FSMPrototype::State& other ) :
		m_id( other.m_id )
	{

	}

	FSMPrototype::State::~State()
	{

	}

	void FSMPrototype::State::Enter() const
	{
		if( m_onEnter )
		{
			m_onEnter();
		}
	}

	void FSMPrototype::State::Exit() const
	{
		if( m_onExit )
		{
			m_onExit();
		}
	}

	void FSMPrototype::SetOnEnter( int id, std::function<void()> on_enter )
	{
		FSMPrototype::State* state = AccessState( id );
		state->m_onEnter = on_enter;
	}

	void FSMPrototype::SetOnExit( int id, std::function<void()> on_exit )
	{
		FSMPrototype::State* state = AccessState( id );
		state->m_onExit = on_exit;
	}

	void FSMPrototype::AddTransition( int from, int to )
	{
		DD_ASSERT( GetState( from ) != nullptr, "From state not registered yet!" );
		DD_ASSERT( GetState( to ) != nullptr, "To state not registered yet!" );

		m_transitions.push_back( std::make_pair( from, to ) );
	}

	void FSMPrototype::AddState( int id )
	{
		m_states.push_back( State( id ) );
	}

	typename FSMPrototype::State* FSMPrototype::AccessState( int id )
	{
		for( State& state : m_states )
		{
			if( state.ID() == id )
			{
				return &state;
			}
		}

		return nullptr;
	}

	typename const FSMPrototype::State* FSMPrototype::GetState( int id ) const
	{
		for( const State& state : m_states )
		{
			if( state.ID() == id )
			{
				return &state;
			}
		}

		return nullptr;
	}

	bool FSMPrototype::HasTransition( int from, int to ) const
	{
		DD_ASSERT( GetState( from ) != nullptr );
		DD_ASSERT( GetState( to ) != nullptr );

		for( const std::pair<int, int>& transition : m_transitions )
		{
			if( transition.first == from && transition.second == to )
			{
				return true;
			}
		}

		return false;
	}

	void FSMPrototype::SetInitialState( int id )
	{
		DD_ASSERT( GetState( id ) != nullptr );

		m_initial = id;
	}

	FSM::FSM( const FSM& other ) :
		m_prototype( other.m_prototype ),
		m_current( other.m_current )
	{

	}

	FSM::FSM( const FSMPrototype& prototype ) :
		m_prototype( prototype )
	{
		m_current = m_prototype.GetInitialState();
	}

	FSM::~FSM()
	{

	}

	bool FSM::TransitionTo( int dest )
	{
		if( !m_prototype.HasTransition( m_current, dest ) )
		{
			return false;
		}

		const FSMPrototype::State* current = m_prototype.GetState( m_current );
		current->Exit();

		m_current = dest;

		const FSMPrototype::State* next = m_prototype.GetState( dest );
		next->Enter();

		return true;
	}

	bool FSM::operator ==( int id ) const
	{
		return m_current == id;
	}
}
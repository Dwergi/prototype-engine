#pragma once

#include "FunctionView.h"

namespace dd
{
	class FSM;

	class FSMState
	{
	public:

		FSMState( const FSMState& other );
		~FSMState();

		void AddTransition( FSMState to, FunctionView<void()> transition );

		int ID() const { return m_id; }

	private:

		int m_id { -1 };
		std::unordered_map<int, FunctionView<void()>> m_transitions;

		friend class FSM;

		FSMState( int id );
	};

	class FSM
	{
	public:

		FSM();
		~FSM();

		void Initialize( int initial_state );

		FSMState& AddState( int id );
		bool TransitionTo( int id );

	private:

		FSMState* m_current;
		std::unordered_map<int, FSMState> m_states;
	};
}
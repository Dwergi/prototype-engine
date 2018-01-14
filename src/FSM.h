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

		void SetOnEnter( FunctionView<void()> on_enter );
		void SetOnExit( FunctionView<void()> on_exit );

		int ID() const { return m_id; }

		void Enter() const;
		void Exit() const;

	private:

		int m_id { -1 };
		FunctionView<void()> m_onEnter;
		FunctionView<void()> m_onExit;

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
		void AddTransition( int from, int to );

		bool TransitionTo( int id );

	private:

		FSMState* m_current { nullptr };
		std::unordered_map<int, FSMState> m_states;
		Vector<std::pair<int, int>> m_transitions;
	};
}
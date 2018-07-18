#pragma once

#include "FunctionView.h"

namespace dd
{
	class FSM
	{
	public:

		FSM();
		~FSM();

		void Initialize( int initial_state );

		void AddState( int id );
		void AddTransition( int from, int to );

		bool TransitionTo( int id );

		void SetOnEnter( int id, FunctionView<void()> on_enter );
		void SetOnExit( int id, FunctionView<void()> on_exit );

	private:

		class State
		{
		public:

			State( const State& other );
			~State();

			int ID() const { return m_id; }

			void Enter() const;
			void Exit() const;

		private:

			friend class FSM;

			int m_id { -1 };
			FunctionView<void()> m_onEnter;
			FunctionView<void()> m_onExit;

			State( int id );
		};

		FSM::State* m_current { nullptr };
		std::unordered_map<int, FSM::State> m_states;
		Vector<std::pair<int, int>> m_transitions;
	};
}
#pragma once

namespace dd
{
	
	struct FSMPrototype
	{
		struct State
		{
			State( int id );
			State( const State& other );
			~State();

			int ID() const { return m_id; }

			void Enter() const;
			void Exit() const;

		private:
			friend struct FSMPrototype;

			int m_id { -1 };
			std::function<void()> m_onEnter;
			std::function<void()> m_onExit;
		};

		void AddTransition( int from, int to );
		bool HasTransition( int from, int to ) const;

		void SetInitialState( int id );
		int GetInitialState() const { return m_initial; }
	
		void AddState( int id );

		State* AccessState( int id );
		const State* GetState( int id ) const;

		void SetOnEnter( int id, std::function<void()> on_enter );
		void SetOnExit( int id, std::function<void()> on_exit );

	private:

		int m_initial { 0 };

		std::vector<State> m_states;
		std::vector<std::pair<int, int>> m_transitions;
	};

	struct FSM
	{
		FSM( const FSMPrototype& prototype );
		FSM( const FSM& other );
		~FSM();
	
		bool TransitionTo( int id );
		bool operator==( int id ) const;

	private:
		const FSMPrototype& m_prototype;
		int m_current { -1 };
	};
}
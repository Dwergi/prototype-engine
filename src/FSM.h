#pragma once

namespace dd
{
	// Example graph for an FSM, used for creating FSM instances.
	template <typename T>
	struct FSMPrototype
	{
		static_assert(std::is_enum_v<T>, "Type must be an enum!");

		struct State
		{
			State(T id);
			State(typename const FSMPrototype<T>::State& other);
			~State();

			T ID() const { return m_id; }

			void Enter() const;
			void Exit() const;

		private:
			friend struct FSMPrototype;

			T m_id;
			std::function<void()> m_onEnter;
			std::function<void()> m_onExit;
		};

		void AddTransition(T from, T to);
		bool HasTransition(T from, T to) const;

		void SetInitialState(T id);
		T GetInitialState() const { return m_initial; }

		void AddState(T id);

		State* AccessState(T id);
		const State* GetState(T id) const;

		void SetOnEnter(T id, std::function<void()> on_enter);
		void SetOnExit(T id, std::function<void()> on_exit);

	private:

		T m_initial;

		std::vector<State> m_states;
		std::vector<std::pair<T, T>> m_transitions;
	};

	template <typename T>
	struct FSM
	{
		FSM(const FSMPrototype<T>& prototype);
		FSM(const FSM<T>& other);
		~FSM();

		bool TransitionTo(T id);
		bool operator==(T id) const;

	private:
		const FSMPrototype<T>& m_prototype;
		T m_current;
	};
}

#include "FSM.inl"
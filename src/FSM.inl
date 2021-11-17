namespace dd
{
	template <typename T>
	FSMPrototype<T>::State::State(T id) :
		m_id(id)
	{

	}

	template <typename T>
	FSMPrototype<T>::State::State(const FSMPrototype<T>::State& other) :
		m_id(other.m_id)
	{

	}

	template <typename T>
	FSMPrototype<T>::State::~State()
	{

	}

	template <typename T>
	void FSMPrototype<T>::State::Enter() const
	{
		if (m_onEnter)
		{
			m_onEnter();
		}
	}

	template <typename T>
	void FSMPrototype<T>::State::Exit() const
	{
		if (m_onExit)
		{
			m_onExit();
		}
	}

	template <typename T>
	void FSMPrototype<T>::SetOnEnter(T id, std::function<void()> on_enter)
	{
		FSMPrototype<T>::State* state = AccessState(id);
		state->m_onEnter = on_enter;
	}

	template <typename T>
	void FSMPrototype<T>::SetOnExit(T id, std::function<void()> on_exit)
	{
		FSMPrototype<T>::State* state = AccessState(id);
		state->m_onExit = on_exit;
	}

	template <typename T>
	void FSMPrototype<T>::AddTransition(T from, T to)
	{
		DD_ASSERT(GetState(from) != nullptr, "From state not registered yet!");
		DD_ASSERT(GetState(to) != nullptr, "To state not registered yet!");

		m_transitions.push_back(std::make_pair(from, to));
	}

	template <typename T>
	void FSMPrototype<T>::AddState(T id)
	{
		m_states.push_back(State(id));
	}

	template <typename T>
	typename FSMPrototype<T>::State* FSMPrototype<T>::AccessState(T id)
	{
		for (State& state : m_states)
		{
			if (state.ID() == id)
			{
				return &state;
			}
		}

		return nullptr;
	}

	template <typename T>
	typename const FSMPrototype<T>::State* FSMPrototype<T>::GetState(T id) const
	{
		for (const State& state : m_states)
		{
			if (state.ID() == id)
			{
				return &state;
			}
		}

		return nullptr;
	}

	template <typename T>
	bool FSMPrototype<T>::HasTransition(T from, T to) const
	{
		DD_ASSERT(GetState(from) != nullptr);
		DD_ASSERT(GetState(to) != nullptr);

		for (const std::pair<T, T>& transition : m_transitions)
		{
			if (transition.first == from && transition.second == to)
			{
				return true;
			}
		}

		return false;
	}

	template <typename T>
	void FSMPrototype<T>::SetInitialState(T id)
	{
		DD_ASSERT(GetState(id) != nullptr);

		m_initial = id;
	}

	template <typename T>
	FSM<T>::FSM(const FSM<T>& other) :
		m_prototype(other.m_prototype),
		m_current(other.m_current)
	{

	}

	template <typename T>
	FSM<T>::FSM(const FSMPrototype<T>& prototype) :
		m_prototype(prototype)
	{
		m_current = m_prototype.GetInitialState();
	}

	template <typename T>
	FSM<T>::~FSM()
	{

	}

	template <typename T>
	bool FSM<T>::TransitionTo(T dest)
	{
		if (!m_prototype.HasTransition(m_current, dest))
		{
			return false;
		}

		const FSMPrototype<T>::State* current = m_prototype.GetState(m_current);
		current->Exit();

		m_current = dest;

		const FSMPrototype<T>::State* next = m_prototype.GetState(dest);
		next->Enter();

		return true;
	}

	template <typename T>
	bool FSM<T>::operator ==(T id) const
	{
		return m_current == id;
	}
}
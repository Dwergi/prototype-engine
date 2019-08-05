#pragma once

namespace dd
{
	template <typename T>
	struct Flags
	{
		static_assert(std::is_integral_v<T>);

		Flags() {}
		Flags(const Flags<T>& other) { m_flags = other.m_flags; }
		Flags(T flag); // intentionally implicit

		void Clear();
		void Fill();

		void Set(T flag);
		void SetAll(Flags<T> flags);

		void Unset(T flag);
		void UnsetAll(Flags<T> flag);

		bool Has(T flag) const;
		bool HasAny(Flags<T> flags) const;
		bool HasAll(Flags<T> flags) const;

		T Value() const { return m_flags; }

	private:
		T m_flags { 0 };
	};

	template <typename T>
	Flags<T>::Flags(T flag)
	{
		DD_ASSERT((T) flag % 2 == 0);
		m_flags = (T) flag;
	}

	template <typename T>
	bool Flags<T>::Has(T flag) const
	{
		DD_ASSERT((T) flag % 2 == 0);
		return (m_flags & (T) flag) != 0;
	}

	template <typename T>
	bool Flags<T>::HasAny(Flags<T> flags) const
	{
		return (m_flags & flags.m_flags) != 0;
	}

	template <typename T>
	bool Flags<T>::HasAll(Flags<T> flags) const
	{
		return (m_flags ^ flags.m_flags) == 0;
	}

	template <typename T>
	void Flags<T>::Set(T flag)
	{
		DD_ASSERT((T) flag % 2 == 0);
		m_flags |= (T) flag;
	}

	template <typename T>
	void Flags<T>::SetAll(Flags<T> flags)
	{
		m_flags |= (T) flags.m_flags;
	}

	template <typename T>
	void Flags<T>::Unset(T flag)
	{
		DD_ASSERT((T) flag % 2 == 0);
		m_flags &= ~(T) flag;
	}

	template <typename T>
	void Flags<T>::UnsetAll(Flags<T> flags)
	{
		m_flags &= ~(T) flags.m_flags;
	}

	template <typename T>
	void Flags<T>::Clear()
	{
		m_flags = 0;
	}

	template <typename T>
	void Flags<T>::Fill()
	{
		m_flags = (T) ~0;
	}
}
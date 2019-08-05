#pragma once

#include <type_traits>

namespace dd
{
	template <typename T>
	struct EnumFlags
	{
		static_assert(std::is_enum_v<T>);
		using TStorage = typename std::underlying_type<T>::type;

		EnumFlags() {}
		EnumFlags(const EnumFlags<T>& other) { m_flags = other.m_flags; }
		EnumFlags(T flag); // intentionally implicit

		void Set(T flag);
		void SetAll(EnumFlags<T> flags);

		void Unset(T flag);
		void UnsetAll(EnumFlags<T> flag);

		bool Has(T flag) const;
		bool HasAll(EnumFlags<T> flags) const;

		TStorage Flags() const { return m_flags; }

		bool operator==(const EnumFlags<T>& other) const { return m_flags == other.m_flags; }

	private:

		TStorage m_flags { 0 };
	};

	template <typename T>
	EnumFlags<T>::EnumFlags(T flag)
	{
		DD_ASSERT((TStorage) flag % 2 == 0);
		m_flags = (TStorage) flag;
	}

	template <typename T>
	bool EnumFlags<T>::Has(T flag) const
	{
		DD_ASSERT((TStorage) flag % 2 == 0);
		return (m_flags & (TStorage) flag) != 0;
	}

	template <typename T>
	bool EnumFlags<T>::HasAll(EnumFlags<T> flags) const
	{
		return (m_flags & flags.m_flags) != 0;
	}

	template <typename T>
	void EnumFlags<T>::Set(T flag)
	{
		DD_ASSERT((TStorage) flag % 2 == 0);
		m_flags |= (TStorage) flag;
	}

	template <typename T>
	void EnumFlags<T>::SetAll(EnumFlags<T> flags)
	{
		m_flags |= (TStorage) flags.m_flags;
	}

	template <typename T>
	void EnumFlags<T>::Unset(T flag)
	{
		DD_ASSERT((TStorage) flag % 2 == 0);
		m_flags &= ~(TStorage) flag;
	}

	template <typename T>
	void EnumFlags<T>::UnsetAll(EnumFlags<T> flags)
	{
		m_flags &= ~(TStorage) flags.m_flags;
	}
}
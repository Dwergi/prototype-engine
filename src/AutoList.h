#pragma once

template <typename T>
class AutoList
{
public:
	AutoList()
	{
		m_next = Head();
		Head() = static_cast<const T *>( this );
	}

	const T* Next() const
	{
		return m_next;
	}

	static const T*& Head()
	{
		static const T* m_head = nullptr;
		return m_head;
	}

private:
	const T* m_next;
};
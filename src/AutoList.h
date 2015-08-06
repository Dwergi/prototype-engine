//
// AutoList.h - A statically-created linked list of all instances of a given type.
// Used most commonly for TypeInfo objects.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
//

#pragma once

namespace dd
{
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
}
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
			Head() = static_cast<T*>( this );
		}

		T* Next() const
		{
			return m_next;
		}

		static T*& Head()
		{
			static T* m_head = nullptr;
			return m_head;
		}

	private:
		T* m_next;
	};
}
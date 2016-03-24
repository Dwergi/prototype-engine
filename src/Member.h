//
// Member.h - Encapsulates a member pointer and associates it with friendly names and stuff.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

namespace dd
{
	class TypeInfo;

	class Member
	{
	public:
		Member();

		//
		// The containing type.
		//
		const TypeInfo* Parent() const;

		//
		// The type of this member.
		//
		const TypeInfo* Type() const;

		//
		// The byte offset of this member relative to its parent class.
		//
		uint Offset() const;

		//
		// The name of this member.
		//
		const String32& Name() const;

	private:
		String32 m_name;
		uint m_offset;
		const TypeInfo* m_typeInfo;
		const TypeInfo* m_parent;
		
		friend class TypeInfo;
	};
}
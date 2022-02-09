//
// Member.h - Encapsulates a member pointer and associates it with friendly names and stuff.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

namespace dd
{
	struct TypeInfo;

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
		uintptr_t Offset() const;

		//
		// The name of this member.
		//
		const String& Name() const;

	private:
		String32 m_name;
		uintptr_t m_offset { 0 };
		const TypeInfo* m_typeInfo { nullptr };
		const TypeInfo* m_parent { nullptr };
		
		friend struct TypeInfo;
	};
}
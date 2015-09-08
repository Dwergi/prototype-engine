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

		const TypeInfo* Type() const;
		uint Offset() const;
		const String32& Name() const;

	private:
		String32 m_name;
		uint m_offset;
		const TypeInfo* m_typeInfo;
		
		friend class TypeInfo;
	};
}
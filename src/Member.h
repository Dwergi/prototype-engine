//
// Member.h - Encapsulates a member pointer and associates it with friendly names and stuff.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once

#define OFFSET_OF( ClassName, MemberName ) ((uint) &(((ClassName*) nullptr)->MemberName))
#define BEGIN_MEMBERS( ClassName ) static void RegisterType() { dd::TypeInfo::Register<ClassName>( #ClassName ); } static void RegisterMembers( std::vector<dd::MemberBase*>& members ) {
#define MEMBER( ClassName, TypeName, MemberName, FriendlyName ) members.emplace_back( new dd::Member<ClassName, TypeName>( &ClassName::MemberName, #MemberName, FriendlyName, #TypeName ) );
#define END_MEMBERS }
//---------------------------------------------------------------------------

namespace dd
{
	class Property;

	class MemberBase
	{
	public:
		std::string m_fieldName;
		std::string m_friendlyName;
		std::string m_typeName;
		uint m_offset;

		MemberBase();
		virtual ~MemberBase();

		virtual void* BindProperty( void* instance ) = 0;
	};
	//---------------------------------------------------------------------------

	template< typename InstanceType, typename ValueType >
	class Member
		: public MemberBase
	{
	public: 
		Member( ValueType InstanceType::* ptr, const char* fieldName, const char* friendlyName, const char* typeName )
			: m_ptr( ptr )
		{
			m_fieldName = fieldName;
			m_friendlyName = friendlyName;
			m_typeName = typeName;
			m_offset = (uint) &(((InstanceType*) nullptr)->*ptr);
		}

		virtual ~Member()
		{
		}

		virtual void* BindProperty( void* instance )
		{
			ValueType& value = static_cast<InstanceType*>( instance )->*m_ptr;
			return &value;
		}

	private:
		ValueType InstanceType::* m_ptr;
	};
}
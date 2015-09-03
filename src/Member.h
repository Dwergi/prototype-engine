//
// Member.h - Encapsulates a member pointer and associates it with friendly names and stuff.
// Copyright (C) Sebastian Nordgren 
// February 17th 2015
//

#pragma once
/*

#define OFFSET_OF( ClassName, MemberName ) ((uint) &(((ClassName*) nullptr)->MemberName))

#define BEGIN_MEMBERS( ClassName ) static void RegisterType() { dd::TypeInfo::Register<ClassName>( #ClassName ); } static void RegisterMembers( dd::Vector<dd::MemberBase*>& members, dd::Vector<dd::MethodBase*>& methods ) {

// Declare a member of the given class.
#define MEMBER( ClassName, TypeName, MemberName, FriendlyName ) members.Allocate( new dd::Member<ClassName, TypeName>( &ClassName::MemberName, #MemberName, FriendlyName, #TypeName ) );

// Declare a script-callable method of the given class. Parameters, if there are multiple, must be submitted in the format "(int, float)". If there is only one, then "int" is fine.
#define METHOD( ClassName, MethodName, ReturnType, ParamType ) methods.Allocate( new dd::Method<ClassName, ReturnType (ClassName::*) (ParamType)>( &ClassName::MethodName, #ClassName, #MethodName, #ReturnType " " #MethodName "(" #ParamType ")" ) );
#define END_MEMBERS }
//---------------------------------------------------------------------------

namespace dd
{
	class Property;

	class MemberBase
	{
	public:
		dd::String m_fieldName;
		dd::String m_friendlyName;
		dd::String m_typeName;
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
		InstanceType* m_instance;
	};
}*/
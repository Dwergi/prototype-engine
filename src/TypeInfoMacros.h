//
// TypeInfoMacros.h - Utility macros to make it less painful to use the TypeInfo system.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
// Heavily inspired by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "RemoveQualifiers.h"

#define UNREFERENCED(P) (P)

// Introspection macros
#define REGISTER_TYPE( TypeName ) \
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( #TypeName )

#define REGISTER_POD( TypeName ) \
	dd::TypeInfo::RegisterPOD<dd::RemoveQualifiers<TypeName>::type>( #TypeName )

#define REGISTER_CONTAINER( Container, Containing ) \
	dd::TypeInfo::RegisterContainer<dd::RemoveQualifiers<Container<Containing>>::type>( #Container, GET_TYPE( Containing ) )

#define GET_TYPE( TypeName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<TypeName>::type>()

#define GET_TYPE_OF( Object ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<decltype( Object )>::type>()

#define GET_TYPE_OF_MEMBER( TypeName, MemberName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<dd::StripMemberness<decltype( &TypeName::MemberName )>::type>::type>()

#define OFFSET_OF( TypeName, MemberName ) \
	((unsigned int) (&((((TypeName*) nullptr))->MemberName)))

#define GET_TYPE_STR( NameString ) \
	dd::TypeInfo::GetType( NameString )

#define BEGIN_TYPE( TypeName ) \
	static void RegisterMembers() { TypeName instance; UNREFERENCED( instance ); dd::TypeInfo* typeInfo = (dd::TypeInfo*) GET_TYPE( TypeName );

#define BEGIN_SCRIPT_OBJECT( TypeName ) \
	dd::RefCounter m_refCount; \
	BEGIN_TYPE( TypeName ) \
	typeInfo->RegisterScriptObject<TypeName>();

#define BEGIN_SCRIPT_STRUCT( TypeName ) \
	BEGIN_TYPE( TypeName ) \
	typeInfo->RegisterScriptStruct<TypeName>();

#define REGISTER_PARENT( TypeName, ParentType ) \
	dd::TypeInfo::AccessType<dd::RemoveQualifiers<TypeName>::type>()->RegisterParentType( GET_TYPE( ParentType ) )

#define PARENT( ParentType ) \
	typeInfo->RegisterParentType( GET_TYPE( ParentType ) )

#define MEMBER( MemberName ) \
	typeInfo->RegisterMember( GET_TYPE_OF( instance.MemberName ), #MemberName, (uint) (reinterpret_cast<uint64>(&instance.MemberName) - reinterpret_cast<uint64>(&instance)) )

#define METHOD( MethodName ) \
	typeInfo->RegisterMethod( FUNCTION( MethodName ), &MethodName, #MethodName )

#define END_TYPE }

#define BASIC_TYPE( TypeName ) \
	static void RegisterMembers() {}

#define REGISTER_SERIALIZERS( TypeName, Serializer, Deserializer ) \
	const_cast<dd::TypeInfo*>( GET_TYPE( TypeName ) )->SetCustomSerializers( Serializer, Deserializer )

#define FUNCTION( FN ) \
	dd::BuildFunction<decltype( &FN ), &FN>( &FN )
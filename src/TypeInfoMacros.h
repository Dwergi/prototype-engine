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
	static void RegisterMembers() { \
	dd::TypeInfo* typeInfo = (dd::TypeInfo*) GET_TYPE( TypeName );

#define BEGIN_SCRIPT_OBJECT( TypeName ) \
	dd::RefCounter m_refCount; \
	BEGIN_TYPE( TypeName ) \
	typeInfo->RegisterScriptType<TypeName, false>();

#define BEGIN_SCRIPT_STRUCT( TypeName ) \
	BEGIN_TYPE( TypeName ) \
	typeInfo->RegisterScriptType<TypeName, true>();

#define REGISTER_PARENT( TypeName, ParentType ) \
	dd::TypeInfo::AccessType<dd::RemoveQualifiers<TypeName>::type>()->RegisterParentType<ParentType>()

#define PARENT( ParentType ) \
	typeInfo->RegisterParentType<ParentType>()

#define MEMBER( TypeName, MemberName ) \
	typeInfo->RegisterMember<TypeName, decltype(MemberName), &TypeName::MemberName>( #MemberName )

#define METHOD( TypeName, MethodName ) \
	typeInfo->RegisterMethod<decltype(&MethodName), &MethodName>( #MethodName )

#define END_TYPE }

#define BASIC_TYPE( TypeName ) \
	static void RegisterMembers() {}

#define REGISTER_SERIALIZERS( TypeName, Serializer, Deserializer ) \
	const_cast<dd::TypeInfo*>( GET_TYPE( TypeName ) )->SetCustomSerializers( Serializer, Deserializer )

// Use this to create a function object to a template function with a comma in the type parameters, eg. add<int, float>
// Required because it doesn't end up matching the signature of FUNCTION otherwise.
#define TEMPLATE_FUNCTION( ... ) FUNCTION( __VA_ARGS__ )

#define FUNCTION( FN ) \
	dd::BuildFunction<decltype( &FN ), &FN>( &FN )

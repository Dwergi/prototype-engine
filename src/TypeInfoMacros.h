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
#define DD_REGISTER_TYPE( TypeName ) \
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( #TypeName )

#define DD_REGISTER_POD( TypeName ) \
	dd::TypeInfo::RegisterPOD<dd::RemoveQualifiers<TypeName>::type>( #TypeName )

#define DD_REGISTER_CONTAINER( Container, Containing ) \
	dd::TypeInfo::RegisterContainer<dd::RemoveQualifiers<Container<Containing>>::type>( #Container, DD_TYPE( Containing ) )

#define DD_TYPE( TypeName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<TypeName>::type>()

#define DD_TYPE_OF( Object ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<decltype( Object )>::type>()

#define DD_TYPE_OF_MEMBER( TypeName, MemberName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<dd::StripMemberness<decltype( &TypeName::MemberName )>::type>::type>()

#define DD_OFFSET_OF( TypeName, MemberName ) \
	((unsigned int) (&((((TypeName*) nullptr))->MemberName)))

#define DD_TYPE_STR( NameString ) \
	dd::TypeInfo::GetType( NameString )

#define DD_BEGIN_TYPE( TypeName ) \
	static void RegisterMembers( dd::TypeInfo* typeInfo ) { \

#define DD_SCRIPT_OBJECT( TypeName ) \
	dd::RefCounter m_refCount; \
	DD_BEGIN_TYPE( TypeName ) \
	typeInfo->RegisterScriptType<TypeName, false>();

#define DD_SCRIPT_STRUCT( TypeName ) \
	DD_BEGIN_TYPE( TypeName ) \
	typeInfo->RegisterScriptType<TypeName, true>();

#define DD_REGISTER_PARENT( TypeName, ParentType ) \
	dd::TypeInfo::AccessType<dd::RemoveQualifiers<TypeName>::type>()->RegisterParentType<ParentType>()

#define DD_PARENT( ParentType ) \
	typeInfo->RegisterParentType<ParentType>();

#define DD_MEMBER( TypeName, MemberName ) \
	typeInfo->RegisterMember<TypeName, decltype(MemberName), &TypeName::MemberName>( #MemberName );

#define DD_METHOD( TypeName, MethodName ) \
	typeInfo->RegisterMethod<decltype(&MethodName), &MethodName>( #MethodName );

#define DD_END_TYPE }

#define DD_BASIC_TYPE( TypeName ) \
	static void RegisterMembers( dd::TypeInfo* typeInfo ) {}

// Use this to create a function object to a template function with a comma in the type parameters, eg. add<int, float>
// Required because it doesn't end up matching the signature of DD_FUNCTION otherwise.
#define TEMPLATE_FUNCTION( ... ) DD_FUNCTION( __VA_ARGS__ )

#define DD_FUNCTION( FN ) \
	dd::BuildFunction<decltype( &FN ), &FN>( &FN )

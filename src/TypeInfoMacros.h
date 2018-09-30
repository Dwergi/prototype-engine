//
// TypeInfoMacros.h - Utility macros to make it less painful to use the TypeInfo system.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
// Heavily inspired by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "RemoveQualifiers.h"

#define UNREFERENCED(P) (P)

// TypeInfo macros
#define DD_REGISTER_CLASS( TypeName ) dd::TypeInfo::RegisterType<TypeName>( #TypeName )

#define DD_REGISTER_POD( TypeName ) dd::TypeInfo::RegisterPOD<TypeName>( #TypeName )

#define DD_REGISTER_CONTAINER( Container, Containing ) dd::TypeInfo::RegisterContainer<dd::RemoveQualifiers<Container<Containing>>::type, Containing>( #Container )

#define DD_TYPE( TypeName ) dd::TypeInfo::GetType<TypeName>()

#define DD_TYPE_OF( Object ) dd::TypeInfo::GetType<dd::RemoveQualifiers<decltype(Object)>::type>()

#define DD_TYPE_OF_MEMBER( TypeName, MemberName ) dd::TypeInfo::GetType<dd::RemoveQualifiers<dd::StripMemberness<decltype(&TypeName::MemberName)>::type>::type>()

#define DD_OFFSET_OF( TypeName, MemberName ) ((unsigned int) (&((((TypeName*) nullptr))->MemberName)))

#define DD_TYPE_STR( NameString ) dd::TypeInfo::GetType( NameString )

#define DD_CLASS( TypeName ) static void RegisterMembers( dd::TypeInfo* typeInfo )

#define DD_COMPONENT() typeInfo->RegisterComponent()

#define DD_REGISTER_PARENT( TypeName, ParentType ) dd::TypeInfo::AccessType<dd::RemoveQualifiers<TypeName>::type>()->RegisterParentType<ParentType>()

#define DD_PARENT( ParentType ) typeInfo->RegisterParentType<ParentType>()

#define DD_MEMBER( TypeName, MemberName ) typeInfo->RegisterMember<TypeName, decltype(MemberName), &TypeName::MemberName>( #MemberName )

#define DD_METHOD( TypeName, MethodName ) typeInfo->RegisterMethod<decltype(&MethodName), &MethodName>( #MethodName )

#define DD_BASIC_TYPE( TypeName ) static void RegisterMembers( dd::TypeInfo* typeInfo ) {}

#define DD_FUNCTION( FN ) dd::BuildFunction<decltype( &FN ), &FN>( &FN )

#define DD_CONCAT_IMPL( A, B ) A##B

#define DD_CONCAT( A, B ) DD_CONCAT_IMPL( A, B )

#define DD_CONCAT_NAMESPACE( A, B ) DD_CONCAT_IMPL( DD_CONCAT_IMPL( A, :: ), B )

#define DD_ENUM( TypeName ) template <> void RegisterEnumOptions<TypeName>( dd::TypeInfo* typeInfo )

#define DD_ENUM_OPTION( TypeName, OptionName ) typeInfo->RegisterEnumOption( DD_CONCAT_NAMESPACE( TypeName, OptionName ), #OptionName )

#define DD_TYPE_CPP( TypeName ) static dd::ClassRegistration<TypeName> DD_CONCAT( s_typeRegistration, __LINE__ )( #TypeName )

#define DD_ENUM_CPP( TypeName ) static dd::EnumRegistration<TypeName> DD_CONCAT( s_typeRegistration, __LINE__ )( #TypeName ); \
	template <> void dd::RegisterEnumOptions<TypeName>( dd::TypeInfo* typeInfo )

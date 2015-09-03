//
// TypeInfoMacros.h - Utility macros to make it less painful to use the TypeInfo system.
// Copyright (C) Sebastian Nordgren 
// September 3rd 2015
// Heavily inspired by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "RemoveQualifiers.h"

// Introspection macros
#define REGISTER_TYPE( TypeName ) \
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( sizeof( TypeName ), #TypeName, false )

#define REGISTER_POD( TypeName ) \
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( sizeof( TypeName ), #TypeName, true )

#define REGISTER_POINTER( TypeName ) \
	dd::TypeInfo::RegisterType<dd::RemoveQualifiers<TypeName>::type>( sizeof( TypeName ), #TypeName, true )

#define GET_TYPE( TypeName ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<TypeName>::type>()

#define GET_TYPE_OF( Object ) \
	dd::TypeInfo::GetType<dd::RemoveQualifiers<decltype( Object )>::type>()

#define GET_TYPE_OF_MEMBER( TypeName, MemberName ) \
	dd::RemoveQualifiers<SEL::StripMemberness<decltype( &TypeName::MemberName )>::type>::type

#define OFFSET_OF( TypeName, MemberName ) \
	((unsigned int) (&((((TypeName*) nullptr))->MemberName)))

#define GET_STR_TYPE( NameString ) \
	dd::TypeInfo::GetType( NameString )

#define ADD_MEMBER( TypeName, MemberName ) \
	((dd::TypeInfo*) GET_TYPE( TypeName ))->AddMember( GET_TYPE( GET_TYPE_OF_MEMBER( TypeName, MemberName ) ), #MemberName, OFFSET_OF( TypeName, MemberName ) )

#define SET_SERIALIZER( TypeName, Serializer ) \
	((dd::TypeInfo*) GET_TYPE( TypeName ))->SetSerializer( Serializer )

#define SET_DESERIALIZER( TypeName, Deserializer ) \
	((dd::TypeInfo*) GET_TYPE( TypeName ))->SetDeserializer( Deserializer )

#define FUNCTION( FN ) \
	dd::BuildFunction<decltype( &FN ), &FN>( &FN )
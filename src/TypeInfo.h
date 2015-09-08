//
// TypeInfo.h - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include "RemoveQualifiers.h"
#include "TypeInfoHelpers.h"
#include "Member.h"

namespace dd
{
	class Variable;
	class TypeInfo;

/*
	typedef void (*SerializeCB)( File&, Variable );
	typedef void (*DeserializeCB)( File&, Variable );*/

	class TypeInfo : public AutoList<TypeInfo>
	{
	public:
		TypeInfo();
		void Init( const char* name, uint size );
		void AddMember( const TypeInfo* typeInfo, const char* name, uint offset );

		inline const dd::Vector<Member>& GetMembers() const { return m_members; }
		const Member* GetMember( const char* memberName ) const;

		inline uint Size() const { return m_size; }
		inline const String32& Name() const { return m_name; }
		inline bool IsPOD() const { return m_isPOD; };

		bool Registered() { return m_size != 0; }

		void* (*New)();
		void (*Copy)( void* data, const void* src );
		void (*Delete)( void* data );
		void (*NewCopy)( void* *dest, const void* src );
		void (*PlacementNew)( void* data );
		void (*PlacementDelete)( void* data );
		void (*PlacementCopy)( void* data, const void* src );

		template<typename T>
		static const TypeInfo* RegisterType( uint size, const char* name );

		template<typename T>
		static const TypeInfo* RegisterPOD( uint size, const char* name );

		template <typename T>
		static const TypeInfo* GetType();
		static const TypeInfo* GetType( const char* typeName );
		static const TypeInfo* GetType( const String32& typeName );

		template <typename T>
		static TypeInfo* AccessType();

		/*
		void SetSerializer( SerializeCB cb );
		void SetDeserializer( DeserializeCB cb );
		void Serialize( File& file, Variable var ) const;
		void Deserialize( File& file, Variable var ) const;
		*/

	private:
		uint m_size;
		String32 m_name;
		Vector<Member> m_members;
		bool m_isPOD;

		static DenseMap<String32, TypeInfo*> sm_typeMap;

		/*
		SerializeCB m_serialize;
		DeserializeCB m_deserialize;

		ToLuaCB m_toLua;
		FromLuaCB m_fromLua;
		const char* m_metatable;
		Array<luaL_Reg> m_luaMethods;

		friend class TextSerializer;
		friend class Engine;*/
	};

	template<typename T>
	const TypeInfo* TypeInfo::GetType()
	{
		static TypeInfo instance;
		return &instance;
	}

	template<typename T>
	TypeInfo* TypeInfo::AccessType()
	{
		return const_cast<TypeInfo*>( GetType<T>() );
	}

	template<typename T>
	const TypeInfo* TypeInfo::RegisterType( uint size, const char* name )
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>( GetType<T>() );
		if( typeInfo->Registered() )
			return typeInfo;

		typeInfo->Init( name, size );
		typeInfo->m_isPOD = false;

		typedef std::conditional<HasDefaultCtor<T>::value, T, EmptyType<T>>::type new_type;
		typeInfo->New = SetFunc<HasDefaultCtor<T>::value, void*(*)(), &dd::New<new_type>>::Get();
		typeInfo->PlacementNew = SetFunc<HasDefaultCtor<T>::value, void (*)( void* ), &dd::PlacementNew<new_type>>::Get();
		typeInfo->Copy = SetFunc<HasCopyCtor<T>::value, void(*)( void*, const void* ), &dd::Copy<new_type>>::Get();
		typeInfo->Delete = dd::Delete<T>;
		typeInfo->PlacementCopy = SetFunc<HasCopyCtor<T>::value, void(*)( void*, const void* ), &dd::PlacementCopy<new_type>>::Get();
		typeInfo->PlacementDelete = dd::PlacementDelete<T>;
		typeInfo->NewCopy = SetFunc<HasCopyCtor<T>::value, void(*)( void**, const void* ), &dd::NewCopy<new_type>>::Get();

		sm_typeMap.Add( name, typeInfo );

		T::RegisterMembers();

		return typeInfo;
	}

	template <typename T>
	const TypeInfo* TypeInfo::RegisterPOD( uint size, const char* name )
	{
		TypeInfo* typeInfo = const_cast<TypeInfo*>( GetType<T>() );
		typeInfo->Init( name, size );
		typeInfo->m_isPOD = true;

		typeInfo->New = PODNew<T>;
		typeInfo->Copy = PODCopy<T>;
		typeInfo->NewCopy = PODNewCopy<T>;
		typeInfo->Delete = PODDelete<T>;
		typeInfo->PlacementNew = PODPlacementNew<T>;
		typeInfo->PlacementDelete = PODPlacementDelete<T>;
		typeInfo->PlacementCopy = PODPlacementCopy<T>;

		sm_typeMap.Add( name, typeInfo );

		return typeInfo;
	}

	static void RegisterDefaultTypes();
};

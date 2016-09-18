//
// TypeInfo.h - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

namespace dd
{
	struct Method
	{
		SharedString Name;
		Function Function;
	};

	class TypeInfo : public AutoList<TypeInfo>
	{
	public:
		TypeInfo();
		void Init( const char* name, uint size );

		const Vector<Member>& Members() const { return m_members; }
		const Member* GetMember( const char* memberName ) const;

		const Function* GetMethod( const char* methodName ) const;

		inline uint Size() const { return m_size; }
		inline const SharedString& Name() const { return m_name; }
		inline const SharedString& Namespace() const { return m_namespace; }
		String128 FullTypeName() const;

		inline bool IsPOD() const { return m_members.Size() == 0 && !IsContainer() && m_parentType == nullptr; }
		inline bool IsRegistered() const { return m_size != 0; }

		bool IsDerivedFrom( const TypeInfo* type ) const;

		//
		// Creation and deletion through a TypeInfo object.
		//
		void* (*New)();
		void (*Copy)( void* data, const void* src );
		void (*Delete)( void* data );
		void (*NewCopy)( void* *dest, const void* src );
		void (*PlacementNew)( void* data );
		void (*PlacementDelete)( void* data );
		void (*PlacementCopy)( void* data, const void* src );

		//
		// Container type accessors.
		//
		inline bool IsContainer() const { return m_containedType != nullptr; }
		inline const TypeInfo* ContainedType() const { return m_containedType; }

		void (*InsertElement)(void* container, uint index, void* item);
		void* (*ElementAt)(void* container, uint index);
		uint (*ContainerSize)(void* container);

		//
		// Register a non-POD, non-container type (eg. a class).
		//
		template <typename T>
		static const TypeInfo* RegisterType( const char* name );

		template <typename TContainer, typename TItem>
		static const TypeInfo* RegisterContainer( const char* container, const TypeInfo* containing );

		template <typename FnType, FnType Fn>
		void RegisterMethod( const char* name );

		template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
		void RegisterMember( const char* name );

		template <typename T>
		void RegisterParentType();

		template <typename T, bool byValue>
		void RegisterScriptType();

		//
		// Register a POD type - these are the basic types like ints, floats and char*.
		// If it can be printed with a printf format string, it's POD.
		//
		template <typename T>
		static const TypeInfo* RegisterPOD( const char* name );

		//
		// TypeInfo accessors. 
		//
		template <typename T>
		static const TypeInfo* GetType();
		static const TypeInfo* GetType( const char* typeName );
		static const TypeInfo* GetType( const String& typeName );
		static const TypeInfo* GetType( const SharedString& typeName );

		template <typename T>
		static TypeInfo* AccessType();

		static void RegisterDefaultTypes();

	private:
		uint m_size;
		SharedString m_namespace;
		SharedString m_name;
		bool m_scriptObject;

		const TypeInfo* m_parentType;
		const TypeInfo* m_containedType;
		
		Vector<Member> m_members;
		Vector<Method> m_methods;

		static bool sm_defaultsRegistered;
		static DenseMap<SharedString, TypeInfo*> sm_typeMap;

		template <typename T>
		void RegisterFunctions();
	};
}

#include "TypeInfo.inl"

//
// TypeInfo.h - Run-time type information implementation.
// Copyright (C) Sebastian Nordgren 
// August 6th 2015
// Heavily influenced by Randy Gaul (http://RandyGaul.net)
//

#pragma once

#include <unordered_map>

namespace dd
{
	typedef uint8 ComponentID;
	static const ComponentID INVALID_COMPONENT = 255;

	struct Method
	{
		String32 Name;
		Function Function;
	};

	struct EnumOption
	{
		String32 Name;
		int Value;
	};

	enum class Category
	{
		Class,
		POD,
		Container,
		Enum
	};

	struct TypeInfo : public AutoList<TypeInfo>
	{
		TypeInfo();
		void Init( const char* name, uint size );

		bool operator==( const TypeInfo& other ) const;

		const Vector<Member>& Members() const { return m_members; }
		const Member* GetMember( const char* memberName ) const;

		const Function* GetMethod( const char* methodName ) const;

		inline uint Size() const { return m_size; }
		inline const String& Name() const { return m_name; }
		inline const String& Namespace() const { return m_namespace; }
		String128 FullTypeName() const;

		inline bool IsRegistered() const { return m_size != 0; }

		inline Category GetCategory() const { return m_category; }
		
		inline bool IsComponent() const { return m_componentID != INVALID_COMPONENT; }
		inline dd::ComponentID ComponentID() const { return m_componentID; }

		bool IsDerivedFrom( const TypeInfo* type ) const;

		template <typename FnType, FnType Fn>
		void RegisterMethod( const char* name );

		template <typename TClass, typename TProp, TProp TClass::* MemberPtr>
		void RegisterMember( const char* name );

		void RegisterComponent();

		template <typename T>
		void RegisterParentType();

		template <typename T>
		void RegisterScriptType();

		template <typename T>
		void RegisterEnumOption( T value, const char* name );
		const Vector<EnumOption>& GetEnumOptions() const { return m_enumOptions; }

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
		inline const TypeInfo* ContainedType() const { return m_containedType; }

		void (*InsertElement)(void* container, uint index, void* item);
		void* (*ElementAt)(void* container, uint index);
		uint (*ContainerSize)(void* container);

		//
		// Register a non-POD, non-container type (eg. a class).
		//
		template <typename T>
		static TypeInfo* RegisterType( const char* name );

		//
		// Register a container type like a vector.
		//
		template <typename TContainer, typename TItem>
		static TypeInfo* RegisterContainer( const char* container );

		//
		// Register a POD type - these are the basic types like ints, floats and char*.
		// If it can be printed with a printf format string, it's POD.
		//
		template <typename T>
		static TypeInfo* RegisterPOD( const char* name );

		//
		// Register an enum type.
		//
		template <typename T>
		static TypeInfo* RegisterEnum( const char* name );

		//
		// TypeInfo accessors. 
		//
		template <typename T>
		static const TypeInfo* GetType();
		static const TypeInfo* GetType( const char* typeName );
		static const TypeInfo* GetType( const String& typeName );

		template <typename T>
		static TypeInfo* AccessType();

		static void QueueRegistration( std::function<void()> fn );
		
		static void RegisterQueuedTypes();
		static void RegisterDefaultTypes();

		static void SetScriptEngine( AngelScriptEngine* scriptEngine );

		static const TypeInfo* GetComponent( dd::ComponentID id );
		static size_t ComponentCount() { return sm_maxComponentID; }

	private:

		String8 m_namespace;
		String64 m_name;

		uint m_size { 0 };

		bool m_scriptObject { false };
		
		Category m_category;
		
		dd::ComponentID m_componentID { INVALID_COMPONENT };

		const TypeInfo* m_parentType { nullptr };
		const TypeInfo* m_containedType { nullptr };
		
		Vector<Member> m_members;
		Vector<Method> m_methods;
		Vector<EnumOption> m_enumOptions;

		static uint8 sm_maxComponentID;

		static bool sm_defaultsRegistered;
		static std::unordered_map<String64, TypeInfo*>* sm_typeMap;
		static std::vector<std::function<void()>>* sm_registrations;

		static AngelScriptEngine* sm_scriptEngine;

		template <typename T>
		void RegisterFunctions();
	};

	template <typename T>
	void RegisterEnumOptions( dd::TypeInfo* typeInfo );

	template <typename T>
	struct ClassRegistration
	{
		ClassRegistration( const char* typeName )
		{
			dd::TypeInfo* type = dd::TypeInfo::RegisterType<T>( typeName );
			auto fn = [type]()
			{ 
				T::RegisterMembers( type );
			};

			dd::TypeInfo::QueueRegistration( fn );
		}
	};

	template <typename T>
	struct EnumRegistration
	{
		EnumRegistration( const char* typeName )
		{
			dd::TypeInfo* type = dd::TypeInfo::RegisterEnum<T>( typeName );
			auto fn = [type]()
			{
				dd::RegisterEnumOptions<T>( type );
			};

			dd::TypeInfo::QueueRegistration( fn );
		}
	};
}

#include "TypeInfo.inl"

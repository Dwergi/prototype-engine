#pragma once

namespace ddc
{
#define DD_COMPONENT const ddc::ComponentType& GetType() const { return Type; } \
	static void Construct( void* ptr ); \
	static const ddc::ComponentType Type

#define DD_COMPONENT_CPP( TypeName ) DD_STATIC_ASSERT( std::is_copy_constructible_v<TypeName> && std::is_default_constructible_v<TypeName> ); \
	void TypeName::Construct( void* ptr ) { new (ptr) TypeName(); } \
	const ddc::ComponentType TypeName::Type( #TypeName, sizeof( TypeName ), &TypeName::Construct )

	static const int MAX_ENTITIES = 1024;
	static const int MAX_COMPONENTS = 32;
	static const int MAX_TAGS = 32;

	typedef int TypeID;

	struct ComponentType
	{
		static ComponentType* Types[ MAX_COMPONENTS ];
		static int Count;

		static const TypeID InvalidID = -1;

		TypeID ID;
		const char* Name;
		size_t Size;
		void( *Construct )(void*);

		ComponentType( const char* name, size_t size, void( *ctor )(void*) )
		{
			Name = name;
			ID = Count;
			Size = size;
			Construct = ctor;

			Types[ ID ] = this;
			++Count;
		}

		bool operator==( const ComponentType& other ) const
		{
			return ID == other.ID;
		}
	};
}
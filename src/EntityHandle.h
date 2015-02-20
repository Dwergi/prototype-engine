#pragma once

#include <functional>

class EntitySystem;

class EntityHandle
{
public:

	int ID;
	int Version;

	EntityHandle();

	bool IsValid() const;

	bool operator==( const EntityHandle& other ) const;
	bool operator!=( const EntityHandle& other ) const;

private: 
	
	friend class EntitySystem;
	friend std::hash<EntityHandle>;

	static const int Invalid = -1;

	EntitySystem* m_system;

	EntityHandle( int id, int version, EntitySystem* m_system );
};

// hash for STL containers
namespace std
{
	template<>
	struct hash<EntityHandle>
	{
		std::size_t operator()( const EntityHandle& entity ) const
		{
			return entity.ID;
		}
	};
}
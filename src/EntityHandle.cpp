#include "PrecompiledHeader.h"

#include "EntityHandle.h"
#include "EntitySystem.h"

EntityHandle::EntityHandle() : 
	m_system( nullptr ),
	ID( EntityHandle::Invalid ),
	Version( EntityHandle::Invalid )
{

}

EntityHandle::EntityHandle( int id, int version, EntitySystem* m_system ) : 
	m_system( m_system ),
	ID( id ),
	Version( version )
{

}

bool EntityHandle::IsValid() const
{
	return m_system->IsEntityValid( *this );
}

bool EntityHandle::operator==( const EntityHandle& other ) const
{
	return ID == other.ID && Version == other.Version;
}

bool EntityHandle::operator!=( const EntityHandle& other ) const
{
	return !(*this == other);
}
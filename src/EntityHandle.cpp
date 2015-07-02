#include "PrecompiledHeader.h"

#include "EntityHandle.h"
#include "EntitySystem.h"

dd::EntityHandle::EntityHandle() : 
	m_system( nullptr ),
	ID( EntityHandle::Invalid ),
	Version( EntityHandle::Invalid )
{

}

dd::EntityHandle::EntityHandle( int id, int version, EntitySystem* m_system ) : 
	m_system( m_system ),
	ID( id ),
	Version( version )
{

}

bool dd::EntityHandle::IsValid() const
{
	return m_system->IsEntityValid( *this );
}

bool dd::EntityHandle::operator==( const dd::EntityHandle& other ) const
{
	return ID == other.ID && Version == other.Version;
}

bool dd::EntityHandle::operator!=( const dd::EntityHandle& other ) const
{
	return !(*this == other);
}
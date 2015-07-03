#include "PrecompiledHeader.h"
#include "SwarmSystem.h"

#include "DoubleBuffer.h"

dd::SwarmSystem::SwarmSystem( const dd::DoubleBuffer< dd::SwarmAgentComponent::Pool >& pool )
	: m_pool( pool )
{

}

dd::SwarmSystem::~SwarmSystem()
{

}

void dd::SwarmSystem::Update( float dt )
{
	auto& read = m_pool.GetRead();
	for( const dd::SwarmAgentComponent& cmp : read )
	{

	}
}
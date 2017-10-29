#include "PrecompiledHeader.h"
#include "ScriptSystem.h"

#include "EntityManager.h"
#include "ScriptComponent.h"

namespace dd
{
	ScriptSystem::ScriptSystem( AngelScriptEngine& scriptEngine )
		: m_scriptEngine( scriptEngine )
	{
	}

	ScriptSystem::~ScriptSystem()
	{
	}

	void ScriptSystem::Update( EntityManager& entityManager, float dt )
	{
		entityManager.ForAllWithWritable<ScriptComponent>( [this]( EntityHandle entity, ComponentHandle<ScriptComponent> component )
		{
			ScriptComponent* writable = component.Write();
			if( !writable->IsLoaded() )
			{
				const String& module = writable->GetModule();
				if( !m_scriptEngine.IsModuleLoaded( module.c_str() ) )
				{
					String256 output;
					bool loaded = m_scriptEngine.LoadFile( module.c_str(), output );
					DD_ASSERT( loaded, output.c_str() );

					writable->SetLoaded( true );
				}
			}
		} );

		entityManager.ForAllWithReadable<ScriptComponent>( [this, dt]( EntityHandle entity, ComponentHandle<ScriptComponent> component )
		{
			const String& module = component.Read()->GetModule();

			dd::AngelScriptFunction* function = m_scriptEngine.GetFunction( module.c_str(), "void Update( float dt )" );
			if( function != nullptr )
			{
				(*function)(dt);
			}
		} );
	}
}
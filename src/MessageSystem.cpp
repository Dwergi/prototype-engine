//
// MessageSystem.cpp - A pub/sub style messaging system.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "MessageSystem.h"

namespace dd
{
	MessageSystem::MessageSystem()
		: m_nextHandlerID( 0 )
	{

	}

	MessageSystem::~MessageSystem()
	{

	}

	MessageSubscription MessageSystem::Subscribe( uint message_type, Function handler )
	{
		const FunctionSignature* sig = handler.Signature();
		const TypeInfo* arg = sig->GetArg( 0 );

		ASSERT( sig->ArgCount() == 1 );
		ASSERT( arg == GET_TYPE( Message* ) );

		MessageSubscription new_token;
		new_token.Handler = m_nextHandlerID++;
		new_token.MessageID = message_type;

		Vector<HandlerID>* existing = m_subscribers.Find( MessageID( message_type ) );
		
		if( existing == nullptr )
		{
			m_subscribers.Add( message_type, Vector<HandlerID>() );
			existing = m_subscribers.Find( message_type );
		}

		existing->Add( new_token.Handler );
		m_handlers.Add( new_token.Handler, std::move( handler ) );

		return new_token;
	}

	void MessageSystem::Unsubscribe( MessageSubscription token )
	{
		m_handlers.Remove( token.Handler );

		Vector<HandlerID>* subs = m_subscribers.Find( token.MessageID );

		for( uint i = 0; i < subs->Size(); ++i )
		{
			if( (*subs)[i] == token.Handler )
			{
				subs->Remove( i );
			}
		}
	}

	void MessageSystem::Send( Message* message )
	{
		Vector<HandlerID>* subs = m_subscribers.Find( message->Type );

		if( subs == nullptr )
			return;

		for( uint i = 0; i < subs->Size(); ++i )
		{
			Function* fn = m_handlers.Find( (*subs)[i] );
			if( fn != nullptr )
			{
				(*fn)(message);
			}
		}
	}

	uint MessageSystem::GetSubscriberCount( uint message_type ) const
	{
		Vector<HandlerID>* subs = m_subscribers.Find( message_type );

		if( subs == nullptr )
			return 0;

		return subs->Size();
	}

	uint MessageSystem::GetTotalSubscriberCount() const
	{
		return m_handlers.Size();
	}
}
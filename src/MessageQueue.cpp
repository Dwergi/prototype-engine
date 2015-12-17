//
// MessageQueue.cpp - A pub/sub style messaging system.
// Messages are queued up sequentially and processed during Update.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PrecompiledHeader.h"
#include "MessageQueue.h"

#include <thread>

namespace dd
{
	MessageQueue::MessageQueue()
		: m_nextHandlerID( 0 )
	{

	}

	MessageQueue::~MessageQueue()
	{

	}

	MessageSubscription MessageQueue::Subscribe( uint message_type, Function handler )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

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

	void MessageQueue::Unsubscribe( MessageSubscription token )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		m_handlers.Remove( token.Handler );

		Vector<HandlerID>* subs = m_subscribers.Find( token.MessageID );

		if( subs == nullptr )
			return;

		for( uint i = 0; i < subs->Size(); ++i )
		{
			if( (*subs)[i] == token.Handler )
			{
				subs->Remove( i );
				break;
			}
		}
	}

	void MessageQueue::Send( Message* message )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		m_pendingMessages.Add( message );
	}

	void MessageQueue::Dispatch( Message* message ) const
	{
		Vector<HandlerID>* subs = m_subscribers.Find( message->Type );

		if( subs == nullptr )
			return;

		for( HandlerID handler : *subs )
		{
			Function* fn = m_handlers.Find( handler );
			if( fn != nullptr )
			{
				(*fn)(message);
			}
		}
	}

	void MessageQueue::Update( float dt )
	{
		std::lock_guard<std::mutex> lock( m_mutex );

		for( Message* message : m_pendingMessages )
		{
			Dispatch( message );
		}

		m_pendingMessages.Clear();
	}

	uint MessageQueue::GetSubscriberCount( uint message_type ) const
	{
		Vector<HandlerID>* subs = m_subscribers.Find( message_type );

		if( subs == nullptr )
			return 0;

		return subs->Size();
	}

	uint MessageQueue::GetTotalSubscriberCount() const
	{
		return m_handlers.Size();
	}
}
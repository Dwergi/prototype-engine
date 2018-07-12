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
		: m_nextHandlerID( 0 ),
		m_pendingMessages( new Vector<Message*>, new Vector<Message*> )
	{

	}

	MessageQueue::~MessageQueue()
	{

	}

	MessageSubscription MessageQueue::Subscribe( MessageID message_type, std::function<void( Message* )> handler )
	{
		std::lock_guard lock( m_mutex );

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
		m_handlers.Add( new_token.Handler, handler );

		return new_token;
	}

	void MessageQueue::Unsubscribe( MessageSubscription token )
	{
		std::lock_guard lock( m_mutex );

		m_handlers.Remove( token.Handler );

		Vector<HandlerID>* subs = m_subscribers.Find( token.MessageID );

		if( subs == nullptr )
			return;

		for( int i = 0; i < subs->Size(); ++i )
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
		std::lock_guard lock( m_mutex );

		m_pendingMessages.GetWrite().Add( message );
	}

	void MessageQueue::Dispatch( Message* message ) const
	{
		Vector<HandlerID>* subs = m_subscribers.Find( message->Type );

		if( subs == nullptr )
			return;

		for( HandlerID handler : *subs )
		{
			std::function<void(Message*)>* fn = m_handlers.Find( handler );
			if( fn != nullptr )
			{
				(*fn)(message);
			}
		}
	}

	void MessageQueue::Update( float dt )
	{
		{
			std::lock_guard lock( m_mutex );

			m_pendingMessages.Swap();
		}

		for( Message* message : m_pendingMessages.GetRead() )
		{
			Dispatch( message );
		}

		m_pendingMessages.GetWrite().Clear();
	}

	int MessageQueue::GetSubscriberCount( MessageID message_type ) const
	{
		Vector<HandlerID>* subs = m_subscribers.Find( message_type );

		if( subs == nullptr )
			return 0;

		return subs->Size();
	}

	int MessageQueue::GetTotalSubscriberCount() const
	{
		return m_handlers.Size();
	}

	int MessageQueue::GetPendingMessageCount() const
	{
		return m_pendingMessages.GetWrite().Size();
	}
}
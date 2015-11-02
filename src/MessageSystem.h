//
// MessageSystem.h - A pub/sub style messaging system.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

#include "Message.h"
#include "MessageTypes.h"

namespace dd
{
	struct MessageSubscription;

	class MessageSystem
	{
	private:
		typedef uint HandlerID;
		typedef uint MessageID;

	public:

		MessageSystem();
		~MessageSystem();

		//
		// Subscribe to a given message with the given handler.
		// If you ever want to unsubscribe, you must keep the returned token.
		//
		MessageSubscription Subscribe( uint message_type, Function handler );

		//
		// Unsubscribe the given token. 
		//
		void Unsubscribe( MessageSubscription token );
	
		//
		// Send a message. The message must be derived from the Message struct.
		//
		void Send( Message* message );

		uint GetSubscriberCount( uint message_type ) const;

		uint GetTotalSubscriberCount() const;

	private:

		DenseMap<MessageID, Vector<HandlerID>> m_subscribers;
		DenseMap<HandlerID, Function> m_handlers;

		HandlerID m_nextHandlerID;
	};

	struct MessageSubscription
	{
	private:

		uint MessageID;
		uint Handler;

		friend class MessageSystem;
	};
}
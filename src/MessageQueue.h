
//
// MessageQueue.h - A pub/sub style messaging system.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

#include "DoubleBuffer.h"
#include "Message.h"
#include "MessageTypes.h"

#include <mutex>

namespace dd
{
	struct MessageSubscription;

	class MessageQueue
	{
	public:

		typedef int HandlerID;
		typedef int MessageID;

		MessageQueue();
		~MessageQueue();

		//
		// Subscribe to a given message with the given handler.
		// If you ever want to unsubscribe, you must keep the returned token.
		//
		MessageSubscription Subscribe( MessageID message_type, std::function<void(Message*)> handler );

		//
		// Unsubscribe the given token. 
		//
		void Unsubscribe( MessageSubscription token );
	
		//
		// Send a message. The message must be derived from the Message struct.
		//
		void Send( Message* message );

		//
		// Actually send messages in order of arrival.
		//
		void Update( float dt );

		//
		// Get the number of subscribers to the given message type.
		//
		int GetSubscriberCount( MessageID message_type ) const;

		//
		// Get the total number of subscribers.
		//
		int GetTotalSubscriberCount() const;

		//
		// Get the number of pending messages.
		//
		int GetPendingMessageCount() const;

	private:

		std::mutex m_mutex;

		DenseMap<MessageID, Vector<HandlerID>> m_subscribers;
		DenseMap<HandlerID, std::function<void( Message* )>> m_handlers;
		DoubleBuffer<Vector<Message*>> m_pendingMessages;

		HandlerID m_nextHandlerID;

		void Dispatch( Message* message ) const;
	};

	struct MessageSubscription
	{
	private:
		int MessageID;
		int Handler;

		friend class MessageQueue;
	};
}
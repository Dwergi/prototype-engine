//
// MessageQueue.h - A pub/sub style messaging system.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

#include "DoubleBuffer.h"
#include "MessageType.h"

#include <mutex>

namespace ddc
{
	struct MessageSubscription;

	typedef uint MessageID;
	typedef int MessageHandlerID;

	struct Message
	{
		MessageType Type { MessageType::Unknown };

		Message();
		Message( const Message& other );

		template <typename T>
		void SetPayload( const T& payload );

		template <typename T>
		const T& GetPayload() const;

		DD_BASIC_TYPE( Message )

	private:
		
		static const size_t PAYLOAD_SIZE = 64;

		byte m_payload[PAYLOAD_SIZE];
		const dd::TypeInfo* m_payloadType { nullptr };
	};

	struct MessageQueue
	{
		MessageQueue();
		~MessageQueue();

		//
		// Subscribe to a given message with the given handler.
		// If you ever want to unsubscribe, you must keep the returned token.
		//
		MessageSubscription Subscribe( MessageType type, std::function<void(Message)> handler );

		//
		// Unsubscribe the given token. 
		//
		void Unsubscribe( MessageSubscription token );
	
		//
		// Send a message. The message must be derived from the Message struct.
		//
		void Send( Message message );

		//
		// Actually send messages in order of arrival.
		//
		void Update();

		//
		// Get the number of subscribers to the given message type.
		//
		int GetSubscriberCount( MessageType type ) const;

		//
		// Get the total number of subscribers.
		//
		int GetTotalSubscriberCount() const { return (int) m_handlers.size(); }

		//
		// Get the number of pending messages.
		//
		int GetPendingMessageCount() const;

	private:

		std::mutex m_mutex;

		std::unordered_map<MessageType, std::vector<MessageHandlerID>> m_subscribers;
		std::unordered_map<MessageHandlerID, std::function<void(Message)>> m_handlers;
		dd::DoubleBuffer<std::vector<Message>> m_pendingMessages;

		MessageHandlerID m_nextHandlerID;

		void Dispatch( const Message& message ) const;
	};

	struct MessageSubscription
	{
	private:
		MessageType Type;
		MessageHandlerID Handler;

		friend struct MessageQueue;
	};

	template <typename T>
	void Message::SetPayload( const T& payload )
	{
		static_assert(sizeof( T ) <= PAYLOAD_SIZE);
		m_payloadType = DD_FIND_TYPE( T );
		memcpy( m_payload, &payload, sizeof( T ) );
	}

	template <typename T>
	const T& Message::GetPayload() const
	{
		DD_ASSERT( m_payloadType == DD_FIND_TYPE( T ) );

		return *reinterpret_cast<const T*>(m_payload);
	}
}
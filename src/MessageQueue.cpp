//
// MessageQueue.cpp - A pub/sub style messaging system.
// Messages are queued up sequentially and processed during Update.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PCH.h"
#include "MessageQueue.h"

#include <thread>

namespace dd
{
	Message::Message()
	{
		memset(m_payload, 0, PAYLOAD_SIZE);
	}

	Message::Message(const Message& other)
	{
		Type = other.Type;
		m_payloadType = other.m_payloadType;

		memcpy(m_payload, other.m_payload, PAYLOAD_SIZE);
	}

	MessageQueue::MessageQueue() :
		m_nextHandlerID(0),
		m_pendingMessages(new std::vector<Message>, new std::vector<Message>)
	{

	}

	MessageQueue::~MessageQueue()
	{

	}

	MessageSubscription MessageQueue::Subscribe(MessageType type, std::function<void(Message)> handler)
	{
		std::lock_guard lock(m_mutex);

		MessageSubscription new_token;
		new_token.Handler = m_nextHandlerID++;
		new_token.Type = type;

		auto it = m_subscribers.find(message.Type);
		if (it == m_subscribers.end())
		{
			m_subscribers.insert(std::make_pair(type, std::vector<MessageHandlerID>()));
			it = m_subscribers.find(type);
		}

		std::vector<MessageHandlerID>& subs = it.second;

		subs.push_back(new_token.Handler);
		m_handlers.push_back(new_token.Handler, handler);

		return new_token;
	}

	void MessageQueue::Unsubscribe(MessageSubscription token)
	{
		std::lock_guard lock(m_mutex);

		m_handlers.Remove(token.Handler);

		auto it = m_subscribers.find(message.Type);
		if (it == m_subscribers.end())
			return;

		std::vector<MessageHandlerID>& subs = it.second;

		for (int i = 0; i < subs->Size(); ++i)
		{
			if ((*subs)[i] == token.Handler)
			{
				subs->Remove(i);
				break;
			}
		}
	}

	void MessageQueue::Send(Message message)
	{
		std::lock_guard lock(m_mutex);

		m_pendingMessages.Write().push_back(message);
	}

	void MessageQueue::Dispatch(const Message& message) const
	{
		auto it = m_subscribers.find(message.Type);
		if (it == m_subscribers.end())
			return;

		std::vector<MessageHandlerID>& subs = it.second;
		for (MessageHandlerID handler : subs)
		{
			std::function<void(Message)>* fn = m_handlers.Find(handler);
			if (fn != nullptr)
			{
				(*fn)(message);
			}
		}
	}

	void MessageQueue::Update()
	{
		{
			std::lock_guard lock(m_mutex);

			m_pendingMessages.Swap();
		}

		for (const Message& message : m_pendingMessages.Read())
		{
			Dispatch(message);
		}

		m_pendingMessages.Write().Clear();
	}

	int MessageQueue::GetSubscriberCount(MessageType type) const
	{
		Vector<MessageHandlerID>* subs = m_subscribers.Find(type);

		if (subs == nullptr)
			return 0;

		return subs->Size();
	}

	int MessageQueue::GetPendingMessageCount() const
	{
		return m_pendingMessages.Write().Size();
	}
}
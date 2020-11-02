//
// MessageQueue.cpp - A pub/sub style messaging system.
// Messages are queued up sequentially and processed during Update.
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#include "PCH.h"
#include "MessageQueue.h"

#include <thread>

namespace ddc
{
	Message::Message()
	{
		memset(m_payload, 0, PAYLOAD_SIZE);
	}

	Message::Message(MessageType type)
	{
		m_type = type;

		memset(m_payload, 0, PAYLOAD_SIZE);
	}

	Message::Message(const Message& other)
	{
		m_type = other.m_type;

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

		auto it = m_subscribers.find(type);
		if (it == m_subscribers.end())
		{
			m_subscribers.insert(std::make_pair(type, std::vector<MessageHandlerID>()));
			it = m_subscribers.find(type);
		}

		std::vector<MessageHandlerID>& subs = it->second;

		subs.push_back(new_token.Handler);
		m_handlers.insert(std::make_pair(new_token.Handler, handler));

		return new_token;
	}

	void MessageQueue::Unsubscribe(MessageSubscription token)
	{
		std::lock_guard lock(m_mutex);

		m_handlers.erase(token.Handler);

		auto it = m_subscribers.find(token.Type);
		if (it == m_subscribers.end())
			return;

		std::vector<MessageHandlerID>& subs = it->second;

		for (int i = 0; i < subs.size(); ++i)
		{
			if (subs[i] == token.Handler)
			{
				subs.erase(subs.begin() + i);
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
		auto it = m_subscribers.find(message.GetType());
		if (it == m_subscribers.end())
			return;

		const std::vector<MessageHandlerID>& subs = it->second;
		for (MessageHandlerID handler_id : subs)
		{
			auto it = m_handlers.find(handler_id);
			if (it != m_handlers.end())
			{
				(it->second)(message);
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

		m_pendingMessages.Write().clear();
	}

	int MessageQueue::GetSubscriberCount(MessageType type) const
	{
		auto it = m_subscribers.find(type);
		if (it == m_subscribers.end())
			return 0;

		return (int) it->second.size();
	}

	int MessageQueue::GetPendingMessageCount() const
	{
		return (int) m_pendingMessages.Write().size();
	}
}
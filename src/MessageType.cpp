//
// MessageType.cpp
// Copyright (C) Sebastian Nordgren 
// October 29th 2020
//

#include "PCH.h"
#include "MessageType.h"

namespace ddc
{
	MessageType MessageType::Unknown;
	
	std::mutex MessageType::s_messagesLock;
	std::vector<MessageTypeRegistration> MessageType::s_messages;

	struct DefaultMessageTypes
	{
		DefaultMessageTypes()
		{
			MessageType::Unknown = MessageType(~0u);
		}
	};

	static DefaultMessageTypes s_defaults;

	MessageType MessageType::Get(std::string_view name)
	{
		std::lock_guard<std::mutex> lock(s_messagesLock);
		
		for (uint i = 0; i < s_messages.size(); ++i)
		{
			if (s_messages[i].Name == name)
			{
				return MessageType(i);
			}
		}
		
		return MessageType::Unknown;
	}

	const dd::TypeInfo* MessageType::GetPayloadType() const
	{
		DD_ASSERT(*this != MessageType::Unknown);

		return s_messages[ID].PayloadType;
	}
}
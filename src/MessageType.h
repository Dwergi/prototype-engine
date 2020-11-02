//
// MessageType.h - A base list of message types. 
// Copyright (C) Sebastian Nordgren 
// November 2nd 2015
//

#pragma once

namespace ddc
{
	struct MessageTypeRegistration
	{
		std::string Name;
		const dd::TypeInfo* PayloadType;
	};

	struct MessageType
	{
		MessageType() : ID(~0u) {}

		static MessageType Unknown;

		template <typename TPayload>
		static MessageType Register(std::string_view name);
		static MessageType Get(std::string_view name);

		const dd::TypeInfo* GetPayloadType() const;

		bool operator==(const MessageType& other) const { return ID == other.ID; }
		bool operator!=(const MessageType& other) const { return ID != other.ID; }

	private:

		MessageType(uint id) : ID(id) {}

		uint ID;

		friend struct DefaultMessageTypes;
		friend struct std::hash<ddc::MessageType>;

		static std::mutex s_messagesLock;
		static std::vector<MessageTypeRegistration> s_messages;

	};

	template <typename TPayload>
	MessageType MessageType::Register(std::string_view name)
	{
		std::lock_guard<std::mutex> lock(s_messagesLock);

		std::string name_str(name);
		for (uint i = 0; i < s_messages.size(); ++i)
		{
			if (s_messages[i].Name == name)
			{
				DD_ASSERT(s_messages[i].PayloadType == DD_FIND_TYPE(TPayload), "Registering message type with different payload type!");

				// already registered, just return it
				return MessageType(i);
			}
		}

		uint id = (uint) s_messages.size();

		ddc::MessageTypeRegistration new_type;
		new_type.Name = name;

		const dd::TypeInfo* payloadType = DD_FIND_TYPE(TPayload);
		DD_ASSERT(payloadType != nullptr);
		new_type.PayloadType = payloadType;

		s_messages.push_back(new_type);

		return id;
	}
}

namespace std
{
	template <>
	struct hash<ddc::MessageType>
	{
		size_t operator()(const ddc::MessageType& t) const
		{
			return t.ID;
		}
	};
}
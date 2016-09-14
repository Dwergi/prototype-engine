//
// Socket.h - UDP socket wrapper.
// Copyright (C) Sebastian Nordgren 
// September 12th 2016
//

#pragma once

namespace sf
{
	class IpAddress;
	class UdpSocket;
}

namespace dd
{
	class Socket;

	class IPAddress
	{
		IPAddress();
		IPAddress( const IPAddress& other );
		IPAddress( const String& address, uint16 port );

		bool Valid() const;

	private:

		friend class Socket;

		sf::IpAddress* m_pAddress;
		uint16 m_port;
	};

	class Socket
	{
	public:

		Socket();
		virtual ~Socket();

		bool Send( const Buffer<byte>& packet, const IPAddress& address );

		//
		// Listen to a port to receive data on it.
		// Should be called before Receive.
		//
		bool Listen( uint16 port );
		bool Receive( Buffer<byte>& packet, uint& read, IPAddress& from );
		
	private:

		sf::UdpSocket* m_pSocket;
		IPAddress m_address;
	};
}
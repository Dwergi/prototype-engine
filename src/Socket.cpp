//
// Socket.h - UDP socket wrapper.
// Copyright (C) Sebastian Nordgren 
// September 12th 2016
//

#include "PrecompiledHeader.h"
#include "Socket.h"
/*

#include "SFML/Network/IpAddress.hpp"
#include "SFML/Network/UdpSocket.hpp"

namespace dd
{
	IPAddress::IPAddress()
	{
		m_pAddress = nullptr;
		m_port = 0;
	}

	IPAddress::IPAddress( const IPAddress& other )
	{
		m_pAddress = new sf::IpAddress( *other.m_pAddress );
		m_port = other.m_port;
	}

	IPAddress::IPAddress( const String& address, uint16 port )
	{
		m_pAddress = new sf::IpAddress( address.c_str() );
		m_port = port;
	}

	bool IPAddress::Valid() const
	{
		return m_pAddress != nullptr && m_port != 0;
	}

	Socket::Socket()
	{
		m_pSocket = new sf::UdpSocket();
		m_pSocket->setBlocking( false );
	}

	Socket::~Socket()
	{
	}

	bool Socket::Send( const Buffer<byte>& packet, const IPAddress& address )
	{
		sf::Socket::Status status = m_pSocket->send( packet.Get(), packet.Size(), *m_address.m_pAddress, m_address.m_port );
		
		return status == sf::Socket::Status::Done;
	}

	bool Socket::Listen( uint16 port )
	{
		sf::Socket::Status status = m_pSocket->bind( port );

		return status == sf::Socket::Status::Done;
	}

	bool Socket::Receive( Buffer<byte>& packet, uint& read, IPAddress& from )
	{
		if( from.m_pAddress == nullptr )
		{
			from.m_pAddress = new sf::IpAddress();
		}

		std::size_t received;
		sf::Socket::Status status = m_pSocket->receive( packet.Get(), packet.Size(), received, *from.m_pAddress, from.m_port );
		read = (uint) received;

		if( status == sf::Socket::Status::NotReady )
			return true;

		return status == sf::Socket::Status::Done;
	}
}*/
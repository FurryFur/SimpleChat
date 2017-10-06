//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

//Library Inludes
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <iostream>

//Local Includes 
#include "network.h"

//This Include
#include "socket.h"


CSocket::CSocket()
	:m_hSocket(0)
{
	ZeroMemory(&m_SocketAddress, sizeof(m_SocketAddress));
}

CSocket::~CSocket()
{
	closesocket(m_hSocket);
}

//Implicit Assumption: We are creating only UDP sockets.....
bool CSocket::Initialise(unsigned short _usPortNumber)
{
	int _iError;
	//Create a UDP socket 
	m_hSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_hSocket)
	{
		_iError = WSAGetLastError();
		ErrorRoutines::PrintWSAErrorInfo(_iError);
		std::cout << "Unable to create socket\n";
		return false;
	}
	else
	{
		std::cout << "Successfully created the socket" << std::endl;
	}

	// now bind the socket to the address and port number supplied.
	m_SocketAddress.sin_family = AF_INET;
	m_SocketAddress.sin_port = htons(_usPortNumber);
	m_SocketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
	//Qs 1 : Change if to while; incrementing the port number every time!
	while (0 != bind(m_hSocket, reinterpret_cast<sockaddr*>(&m_SocketAddress), sizeof(m_SocketAddress)))
	{
		// Cannot bind to the specified IP and port; 
		//Try with an incremented port number
		_usPortNumber++;

		m_SocketAddress.sin_port = htons(_usPortNumber);

	}
	std::cout << "Successfully bound the socket " << "to " << GetLocalAddress() << ":" << _usPortNumber << std::endl;
	
	return true;
}

SOCKET CSocket::GetSocketHandle()
{
	return m_hSocket;
}

std::string CSocket::GetLocalAddress()
{
	char _pcLocalHostName[256];
	char _pcLocalAddress[256];
	struct sockaddr_in* sockaddr_localIP;

	gethostname(_pcLocalHostName, 256);

	struct addrinfo hints, *result, *p;
	int _iReturnValue;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;


	_iReturnValue = getaddrinfo(_pcLocalHostName, NULL, &hints, &result);

	/* loop over all returned results to get the address */
	for (p = result; p != NULL; p = p->ai_next) {
		sockaddr_localIP = (struct sockaddr_in *) p->ai_addr;
		inet_ntop(AF_INET, &(sockaddr_localIP->sin_addr), _pcLocalAddress, 256);
	}

	return std::string(_pcLocalAddress);
}
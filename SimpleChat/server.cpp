//
// (c) 2015 Media Design School
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

//Library Includes
#include <WS2tcpip.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <memory>

//Local Includes
#include "utils.h"
#include "network.h"
#include "consoletools.h"
#include "socket.h"


//Local Includes
#include "server.h"

CServer::CServer()
	:m_recvBuffer(0),
	m_pServerSocket(0)
{
}

CServer::~CServer()
{
	delete m_pConnectedClients;
	m_pConnectedClients = 0;

	delete m_pServerSocket;
	m_pServerSocket = 0;

	delete m_pWorkQueue;
	m_pWorkQueue = 0;
	
	delete[] m_recvBuffer;
	m_recvBuffer = 0;
}

bool CServer::Initialise()
{
	m_recvBuffer = new char[MAX_MESSAGE_LENGTH];
	
	//Create a work queue to distribute messages between the main  thread and the receive thread.
	m_pWorkQueue = new AtomicQueue<std::unique_ptr<TPacket>>();

	//Create a socket object
	m_pServerSocket = new CSocket();

	//Get the port number to bind the socket to
	unsigned short _usServerPort = QueryPortNumber(DEFAULT_SERVER_PORT);

	//Initialise the socket to the local loop back address and port number
	if (!m_pServerSocket->Initialise(_usServerPort))
	{
		return false;
	}

	//Qs 2: Create the map to hold details of all connected clients
	m_pConnectedClients = new std::map < std::string, TClientDetails >() ;

	return true;
}

bool CServer::AddClient(const sockaddr_in& address, std::string _strClientName)
{
	//TO DO : Add the code to add a client to the map here...
	
	for (auto it = m_pConnectedClients->begin(); it != m_pConnectedClients->end(); ++it)
	{
		//Check to see that the client to be added does not already exist in the map, 
		if(it->first == ToString(address))
		{
			return false;
		}
		//also check for the existence of the username
		else if (it->second.m_strName == _strClientName)
		{
			return false;
		}
	}
	//Add the client to the map.
	TClientDetails _clientToAdd;
	_clientToAdd.m_strName = _strClientName;
	_clientToAdd.m_ClientAddress = address;

	std::string _strAddress = ToString(address);
	m_pConnectedClients->insert(std::pair < std::string, TClientDetails > (_strAddress, _clientToAdd));
	return true;
}

bool CServer::SendData(char* dataToSend, const sockaddr_in& address)
{
	int _iBytesToSend = (int)strlen(dataToSend) + 1;
	
	int iNumBytes = sendto(
		m_pServerSocket->GetSocketHandle(),				// socket to send through.
		dataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<const sockaddr*>(&address),	// address to be filled with packet target
		sizeof(address)							// size of the above address struct.
		);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

void CServer::ReceiveData()
{
	sockaddr_in fromAddress; // Make a local variable to extract the IP and port number of the sender from whom we are receiving
							  //In this case; it should be the details of the server; since the client only ever receives from the server
	int iSizeOfAdd = sizeof(fromAddress);
	int _iNumOfBytesReceived;

	while (true)
	{
		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			m_pServerSocket->GetSocketHandle(),						// client-end socket being used to read from
			m_recvBuffer,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&fromAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
		);
		if (_iNumOfBytesReceived < 0)
		{
			int _iError = WSAGetLastError();
			ErrorRoutines::PrintWSAErrorInfo(_iError);
			//return false;
		}
		else
		{
			char _IPAddress[100];
			inet_ntop(AF_INET, &fromAddress.sin_addr, _IPAddress, sizeof(_IPAddress));
			
			std::cout << "Server Received \"" << m_recvBuffer << "\" from " <<
				_IPAddress << ":" << ntohs(fromAddress.sin_port) << std::endl;
			
			//Push this packet data into the WorkQ
			std::unique_ptr<TPacket> packet = std::make_unique<TPacket>();
			packet->Deserialize(m_recvBuffer);
			packet->FromAddress = fromAddress;
			m_pWorkQueue->push(std::move(packet));
		}
		//std::this_thread::yield();
		
	} //End of while (true)
}

void CServer::GetRemoteIPAddress(TPacket& packet, char *_pcSendersIP)
{
	char _temp[MAX_ADDRESS_LENGTH];
	int _iAddressLength;
	inet_ntop(AF_INET, &(packet.FromAddress.sin_addr), _temp, sizeof(_temp));
	_iAddressLength = static_cast<int>(strlen(_temp)) + 1;
	strcpy_s(_pcSendersIP, _iAddressLength, _temp);
}

unsigned short CServer::GetRemotePort(const TPacket& packet)
{
	return ntohs(packet.FromAddress.sin_port);
}

void CServer::ProcessData(TPacket& packetRecvd)
{
	TPacket packetToSend;
	switch (packetRecvd.MessageType)
	{
	case HANDSHAKE:
	{
		std::cout << "Server received a handshake message " << std::endl;
		if (AddClient(packetRecvd.FromAddress, packetRecvd.MessageContent))
		{
			//Qs 3: To DO : Add the code to do a handshake here
		}
		break;
	}
	case DATA:
	{
		packetToSend.Serialize(DATA, packetRecvd.MessageContent);
		SendData(packetToSend.PacketData, packetRecvd.FromAddress);

		//std::this_thread::sleep_for(std::chrono::milliseconds(50));

		packetToSend.Serialize(DATA, "TEST MESSAGE");
		SendData(packetToSend.PacketData, packetRecvd.FromAddress);

		break;
	}

	case BROADCAST:
	{
		std::cout << "Received a broadcast packet" << std::endl;
		//Just send out a packet to the back to the client again which will have the server's IP and port in it's sender fields
		packetToSend.Serialize(BROADCAST, "I'm here!");
		SendData(packetToSend.PacketData, packetRecvd.FromAddress);
		break;
	}

	default:
		break;

	}
}

AtomicQueue<std::unique_ptr<TPacket>>* CServer::GetWorkQueue()
{
	return m_pWorkQueue;
}

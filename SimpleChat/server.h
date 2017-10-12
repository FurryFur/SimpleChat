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

#ifndef __SERVER_H__
#define __SERVER_H__

// Library Includes
#include <Windows.h>
#include <map>
#include <time.h>
#include <memory>
#include <chrono>

// Local Includes
#include "networkentity.h"
#include "AtomicQueue.h"
#include "socket.h"

// Types

// Constants

//Forward Declaration
class CSocket;

//Structure to hold the details of all connected clients
struct TClientDetails
{
	sockaddr_in clientAddress;
	//bool m_bIsActive;
	std::string username;
	std::chrono::time_point<std::chrono::steady_clock> lastHeartbeat;
	//time_t m_timeOfLastMessage;
};

using ClientItT = std::map<std::string, TClientDetails>::iterator;

class CServer : public INetworkEntity
{
public:
	// Default Constructors/Destructors
	CServer();
	~CServer();

	// Virtual Methods from the Network Entity Interface.
	virtual bool Initialise() override; //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* dataToSend, const sockaddr_in& address) override;
	virtual void ReceiveData() override;
	virtual void ProcessData(TPacket& packetRecvd) override;
	virtual void GetRemoteIPAddress(TPacket& packet, char* sendersIP) override;
	virtual unsigned short GetRemotePort(const TPacket& packet) override;

	virtual void checkHeartbeats() override;
	ClientItT disconnectClient(ClientItT clientIt);

	AtomicQueue<std::unique_ptr<TPacket>>* GetWorkQueue();
	//Qs 2: Function to add clients to the map.
private:
	bool AddClient(const sockaddr_in& address, std::string _strClientName);

	//A Buffer to contain all packet data for the server
	char* m_recvBuffer;
	//A server has a socket object to create the UDP socket at its end.
	CSocket* m_pServerSocket;

	//Qs 2 : Make a map to hold the details of all the client who have connected. 
	//The structure maps client addresses to client details
	std::map<std::string, TClientDetails>* m_connectedClients;

	//A workQueue to distribute messages between the main thread and Receive thread.
	AtomicQueue<std::unique_ptr<TPacket>>* m_pWorkQueue;
};

#endif
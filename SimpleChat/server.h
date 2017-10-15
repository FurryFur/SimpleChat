//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A chat server.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
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

	// Initialises the server.
	virtual bool Initialise() override; //Implicit in the intialization is the creation and binding of the socket

	// Sends a null terminated message to the specified address using UDP.
	virtual bool SendData(char* dataToSend, const sockaddr_in& address) override;

	// Receive loop for receiving data from the network.
	// Intended to be run in a separate thread.
	// This function stores incoming packets in a queue for later processing.
	virtual void ReceiveData() override;

	// Processes the supplied packet.
	virtual void ProcessData(TPacket& packetRecvd) override;

	// Gets the remote address of the sender from a received packet.
	// Stores the address in the 'sendersIP' output variable.
	virtual void GetRemoteIPAddress(TPacket& packet, char* sendersIP) override;

	// Gets the remote port of the sender from a received packet.
	virtual unsigned short GetRemotePort(const TPacket& packet) override;

	// Checks the heartbeat signals from the clients.
	// If they have lost connection then they are removed from the server.
	virtual void checkHeartbeat() override;

	// Disconnect and remove a client from the server.
	ClientItT disconnectClient(ClientItT clientIt);

	// Returns a pointer to the work queue.
	AtomicQueue<std::unique_ptr<TPacket>>* GetWorkQueue();

private:
	bool AddClient(const sockaddr_in& address, const std::string& _strClientName);

	// A Buffer to contain all packet data for the server
	char* m_recvBuffer;
	// A server has a socket object to create the UDP socket at its end.
	CSocket* m_pServerSocket;

	// The structure maps client addresses to client details
	std::map<std::string, TClientDetails>* m_connectedClients;

	// A workQueue to distribute messages between the main thread and Receive thread.
	AtomicQueue<std::unique_ptr<TPacket>>* m_pWorkQueue;
};

#endif
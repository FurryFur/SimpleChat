//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A chat client.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#ifndef __CLIENT_H__
#define __CLIENT_H__

// Library Includes
#include <string>
#include <memory>

// Local Includes
#include "networkentity.h"
#include "AtomicQueue.h"
#include "socket.h"

// Types

// Constants


//Forward Declaration
class CSocket;

class CClient : public INetworkEntity
{
public:
	// Default Constructors/Destructors
	CClient();
	~CClient();

	// Methods
	
	// Initializes the client and sets up the connection to the server.
	virtual bool Initialise() override; //Implicit in the intialization is the creation and binding of the socket

	// Sends a null terminated message to the specified address using UDP.
	virtual bool SendData(char* dataToSend, const sockaddr_in& address) override;

	// Send data to the server using UDP.
	bool SendData(char* dataToSend);

	// Receive loop for receiving data from the network.
	// Intended to be run in a separate thread.
	// This function stores incoming packets in a queue for later processing.
	virtual void ReceiveData() override;

	// Processes the supplied packet.
	virtual void ProcessData(TPacket& packetRecvd) override;

	// Gets the remote address of the sender from a received packet.
	// Stores the address in the 'sendersIP' output variable.
	virtual void GetRemoteIPAddress(TPacket& packet, char* sendersIP) override;

	// Gets the remote ip address of the server.
	void GetRemoteIPAddress(char* sendersIP);

	// Gets the remote port of the sender from a received packet.
	virtual unsigned short GetRemotePort(const TPacket& packet) override;

	// Gets the remote port of the servers.
	unsigned short GetRemotePort();

	// Checks the heartbeat signal from the server.
	// Make sure we haven't lost connection.
	virtual void checkHeartbeat() override;

	// Attempt to reconnect to the server.
	void attemptReconnect();

	// Send a heartbeat signal to the server.
	void doHeartbeat();

	// Shutdown the client. This stops the client from receiving data
	// on the thread executing the ReceiveData function.
	void terminateClient(const std::string& msg);

	// Returns a pointer to the work queue.
	AtomicQueue<std::unique_ptr<TPacket>>* GetWorkQueue();

	// Broadcasts on the default server port for servers on the LAN.
	// This function wait a short amount of time for servers to respond
	// and will then return true if any servers are found.
	bool BroadcastForServers();

private:
	// This function waits for a short amount of time for a broadcast
	// response from a server.
	void ReceiveBroadcastMessages(char* _pcBufferToReceiveData);

private:
	// A buffer to contain all packet data for the client
	char* m_recvBuffer;
	// A client has a socket object to create the UDP socket at its end.
	CSocket* m_pClientSocket;
	// A Sockaddress structure which will have the details of the server 
	sockaddr_in m_ServerSocketAddress;
	// A username to associate with a client
	char m_cUserName[50];
	// A workQueue to distribute messages between the main thread and Receive thread.
	AtomicQueue<std::unique_ptr<TPacket>>* m_pWorkQueue;

	//A vector to hold all the servers found after broadcasting
	std::vector<sockaddr_in> m_vecServerAddr;
	bool m_bDoBroadcast;
	bool m_connectionEstablished;
	std::string m_username;
	bool m_reconnecting;
	size_t m_reconnectCount;
	size_t m_maxReconnectAttempts;
	void recordHeartbeat();
	void setHeartbeatInterval(std::chrono::milliseconds);
	std::chrono::time_point<std::chrono::steady_clock> m_lastHeartbeatRecvd;
	std::chrono::time_point<std::chrono::steady_clock> m_lastHeartbeatSent;
	std::chrono::milliseconds m_heartbeatInterval;
};

#endif
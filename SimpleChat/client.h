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
	
	virtual bool Initialise() override; //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* dataToSend, const sockaddr_in& address) override;
	bool SendData(char* dataToSend);
	virtual void ReceiveData() override;
	virtual void ProcessData(TPacket& packetRecvd) override;
	virtual void GetRemoteIPAddress(TPacket& packet, char* sendersIP) override;
	void GetRemoteIPAddress(char* sendersIP);
	virtual unsigned short GetRemotePort(const TPacket& packet) override;
	unsigned short GetRemotePort();

	void GetPacketData(char* _pcLocalBuffer);
	AtomicQueue<std::unique_ptr<TPacket>>* GetWorkQueue();

	//Qs7 : Broadcast to Detect Servers
	bool BroadcastForServers();

private:
	// Question 7 : Broadcast to Detect Servers
	void ReceiveBroadcastMessages(char* _pcBufferToReceiveData);

private:
	//A buffer to contain all packet data for the client
	char* m_recvBuffer;
	//A client has a socket object to create the UDP socket at its end.
	CSocket* m_pClientSocket;
	// A Sockaddress structure which will have the details of the server 
	sockaddr_in m_ServerSocketAddress;
	//A username to associate with a client
	char m_cUserName[50];
	//A workQueue to distribute messages between the main thread and Receive thread.
	AtomicQueue<std::unique_ptr<TPacket>>* m_pWorkQueue;

	//Question 7
	//A vector to hold all the servers found after broadcasting
	std::vector<sockaddr_in> m_vecServerAddr;
	bool m_bDoBroadcast;
	bool m_connectionEstablished;
};

#endif
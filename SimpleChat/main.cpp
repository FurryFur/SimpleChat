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

//Library Includes
#include <Windows.h>
#include <cassert>
//#include <vld.h>
#include <thread>

//Local Includes
#include "consoletools.h"
#include "network.h"
#include "client.h"
#include "server.h"
#include "InputLineBuffer.h"
#include <functional>

// make sure the winsock lib is included...
#pragma comment(lib,"ws2_32.lib")

int main()
{
	unsigned char _ucChoice;
	EEntityType _eNetworkEntityType;
	CInputLineBuffer _InputBuffer(MAX_MESSAGE_LENGTH);
	std::thread clientReceiveThread, serverReceiveThread;

	//Get the instance of the network
	CNetwork& rNetwork = CNetwork::GetInstance();
	rNetwork.StartUp();

	//A pointer to hold a client instance
	CClient* client = nullptr;
	//A pointer to hold a server instance
	CServer* server = nullptr;

	// query, is this to be a client or a server?
	_ucChoice = QueryOption("Do you want to run a client or server (C/S)?", "CS");
	switch (_ucChoice)
	{
	case 'C':
	{
		_eNetworkEntityType = CLIENT;
		break;
	}
	case 'S':
	{
		_eNetworkEntityType = SERVER;
		break;
	}
	default:
	{
		std::cout << "This is not a valid option" << std::endl;
		return 0;
		break;
	}
	}
	if (!rNetwork.Initialise(_eNetworkEntityType))
	{
		std::cout << "Unable to initialise the Network........Press any key to continue......";
		_getch();
		return 0;
	}

	//Run receive on a separate thread so that it does not block the main client thread.
	if (_eNetworkEntityType == CLIENT) //if network entity is a client
	{
		client = static_cast<CClient*>(rNetwork.GetNetworkEntity());
		clientReceiveThread = std::thread(&CClient::ReceiveData, client);
	}

	//Run receive of server also on a separate thread 
	else if (_eNetworkEntityType == SERVER) //if network entity is a server
	{
		server = static_cast<CServer*>(rNetwork.GetNetworkEntity());
		serverReceiveThread = std::thread(&CServer::ReceiveData, server);
	}

	while (rNetwork.IsOnline())
	{
		if (_eNetworkEntityType == CLIENT) //if network entity is a client
		{
			client = static_cast<CClient*>(rNetwork.GetNetworkEntity());

			//Prepare for reading input from the user
			_InputBuffer.PrintToScreenTop();

			//Get input from the user
			if (_InputBuffer.Update())
			{
				// we completed a message, lets send it:
				int _iMessageSize = static_cast<int>(strlen(_InputBuffer.GetString()));

				//Put the message into a packet structure
				TPacket _packet;
				_packet.Serialize(DATA, const_cast<char*>(_InputBuffer.GetString())); //Hardcoded username; change to name as taken in via user input.
				client->SendData(_packet.PacketData);
				//Clear the Input Buffer
				_InputBuffer.ClearString();
				//Print To Screen Top
				_InputBuffer.PrintToScreenTop();
			}

			if (client != nullptr)
			{
				client->doHeartbeat();
				client->checkHeartbeats();
				//If the message queue is empty 
				if (client->GetWorkQueue()->empty())
				{
					//Don't do anything
				}
				else
				{
					//Retrieve off a message from the queue and process it
					std::unique_ptr<TPacket> packetRecvd;
					client->GetWorkQueue()->pop(packetRecvd);
					client->ProcessData(*packetRecvd);
				}

				if (!client->IsOnline()) {
					rNetwork.ShutDown();
				}
			}
		}
		else //if you are running a server instance
		{

			if (server != nullptr)
			{
				server->checkHeartbeats();

				if (!server->GetWorkQueue()->empty())
				{
					//Retrieve off a message from the queue and process it
					std::unique_ptr<TPacket> packetRecvd;
					server->GetWorkQueue()->pop(packetRecvd);
					server->ProcessData(*packetRecvd);
				}

				if (!server->IsOnline()) {
					rNetwork.ShutDown();
				}
			}
		}


	} //End of while network is Online

	if (client)
		clientReceiveThread.join();
	if (server)
		serverReceiveThread.join();

	//Shut Down the Network
	if (rNetwork.IsOnline())
		rNetwork.ShutDown();
	CNetwork::DestroyInstance();

	return 0;
}
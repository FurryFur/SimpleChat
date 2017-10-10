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
	std::thread _ClientReceiveThread, _ServerReceiveThread;

	//Get the instance of the network
	CNetwork& _rNetwork = CNetwork::GetInstance();
	_rNetwork.StartUp();

	//A pointer to hold a client instance
	CClient* client = nullptr;
	//A pointer to hold a server instance
	CServer* _pServer = nullptr;

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
	if (!_rNetwork.GetInstance().Initialise(_eNetworkEntityType))
	{
		std::cout << "Unable to initialise the Network........Press any key to continue......";
		_getch();
		return 0;
	}

	//Run receive on a separate thread so that it does not block the main client thread.
	if (_eNetworkEntityType == CLIENT) //if network entity is a client
	{

		client = static_cast<CClient*>(_rNetwork.GetInstance().GetNetworkEntity());
		_ClientReceiveThread = std::thread(&CClient::ReceiveData, client);

	}

	//Run receive of server also on a separate thread 
	else if (_eNetworkEntityType == SERVER) //if network entity is a server
	{

		_pServer = static_cast<CServer*>(_rNetwork.GetInstance().GetNetworkEntity());
		_ServerReceiveThread = std::thread(&CServer::ReceiveData, _pServer);

	}

	while (_rNetwork.IsOnline())
	{
		if (_eNetworkEntityType == CLIENT) //if network entity is a client
		{
			client = static_cast<CClient*>(_rNetwork.GetInstance().GetNetworkEntity());

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
			}

		}
		else //if you are running a server instance
		{

			if (_pServer != nullptr)
			{
				if (!_pServer->GetWorkQueue()->empty())
				{
					//Retrieve off a message from the queue and process it
					std::unique_ptr<TPacket> packetRecvd;
					_pServer->GetWorkQueue()->pop(packetRecvd);
					_pServer->ProcessData(*packetRecvd);
				}
			}
		}


	} //End of while network is Online

	_ClientReceiveThread.join();
	_ServerReceiveThread.join();

	//Shut Down the Network
	_rNetwork.ShutDown();
	_rNetwork.DestroyInstance();

	return 0;
}
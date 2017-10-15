//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : A class for setting up the network.
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#ifndef __NETWORK_H__
#define __NETWORK_H__

//Types
enum EEntityType
{
	CLIENT = 1,
	SERVER
};

//constants
namespace
{
	unsigned const DEFAULT_SERVER_PORT = 50012;
	unsigned const DEFAULT_CLIENT_PORT = 60013;
	unsigned const MAX_MESSAGE_LENGTH = 256;
	unsigned const MAX_ADDRESS_LENGTH = 32;
}

namespace ErrorRoutines
{
	void PrintWSAErrorInfo(int iError);
}

//Forward Decalarations
class INetworkEntity;

class CNetwork
{
public:
	~CNetwork();
	
	// Initilizes the contained network entity as either a server
	// or a client.
	bool Initialise(EEntityType _eType);

	// Initialize the use of windows sockets.
	// Must be called before using socket functions.
	void StartUp(); //A network has an ability to start up

	// Cleans up windows sockets.
	void ShutDown(); //& an ability to be shut down
	
	// Returns true if the network is available.
	bool IsOnline();

	// Retuns the network entity (either server or client).
	INetworkEntity* GetNetworkEntity();

	// Get the singleton instance of this network.
	static CNetwork& GetInstance();

	// Destroy the singleton instance.
	static void DestroyInstance();

private:
	//Make the network class a singleton. There is only one instance of the network running
	CNetwork();
	CNetwork(const CNetwork& _kr);
	CNetwork& operator= (const CNetwork& _kr);

protected:
	//A network has a network entity
	INetworkEntity* m_pNetworkEntity;
	bool m_isOnline;

	// Singleton Instance
	static CNetwork* s_pNetwork;
};
#endif
//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : Base class for a network entity (server or client)
// Author       : Lance Chaney
// Mail         : lance.cha7337@mediadesign.school.nz
//

#ifndef __NETWORKENTITY_H__
#define __NETWORKENTITY_H__

#include <string>
#include <sstream>
#include <chrono>

#include "socket.h"

enum EMessageType : unsigned char
{
	HANDSHAKE,
	DATA,
	KEEPALIVE,
	COMMAND,
	BROADCAST,
	HEARTBEAT,
	HEARTBEAT_TIMEOUT,
	ERROR_USERNAME_TAKEN,
	ERROR_UNKNOWN_CLIENT,
	USER_JOINED,
	ERROR_RECEIVING,
	CONNECTION_CLOSE,
	USER_DISCONNECTED,
	COMMAND_DISPLAY_COMMANDS,
	COMMAND_NOT_RECOGNIZED
};

struct TPacket 
{
	unsigned short MessageType;
	char MessageContent[50];
	char PacketData[60];
	unsigned short PacketSize;
	sockaddr_in FromAddress;

	//void set_packet(short _x, short _y, WORD _object_type, short _object_index, WORD _param)
	void Serialize(EMessageType _MessageType, const char* _message)
	{
		
		MessageType = _MessageType;
		strcpy_s(MessageContent, strlen(_message) + 1, _message);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << MessageContent;
		
		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = static_cast<unsigned short>(_strToSend.size());
	}
	
	TPacket Deserialize(const char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);
		
		iss >> this->MessageType;
		iss.get(); // Remove whitespace
		
		//iss >> this->MessageContent;
		std::string _tempMessageBuffer;
		getline(iss, _tempMessageBuffer, '\0');
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());
		
		return *this;
	}

};

class INetworkEntity
{
public:
	INetworkEntity();

	// Initialises the network entity.
	virtual bool Initialise() = 0; //Implicit in the intialization is the creation and binding of the socket

	// Sends a null terminated message to the specified address using UDP.
	virtual bool SendData(char* dataToSend, const sockaddr_in& address) = 0;

	// Receive loop for receiving data from the network.
	// Intended to be run in a separate thread.
	// This function stores incoming packets in a queue for later processing.
	virtual void ReceiveData() = 0;

	// Processes the supplied packet.
	virtual void ProcessData(TPacket& packet) = 0;

	// Gets the remote address of the sender from a received packet.
	// Stores the address in the 'sendersIP' output variable.
	virtual void GetRemoteIPAddress(TPacket& packet, char* sendersIP) = 0;

	// Gets the remote port of the sender from a received packet.
	virtual unsigned short GetRemotePort(const TPacket& packet) = 0;

	// Returns whether the entity is receiving data.
	virtual bool IsOnline();

	// Checks the heartbeat signals from connected entities.
	// If no heartbeats are received within a period then the
	// entity should be assumed to be disconnected.
	virtual void checkHeartbeat() = 0;

	// Sets the interval for which an entity will be considered
	// disconect if it has not received a heartbeat within that
	// time.
	virtual void setHeartbeatTimeout(std::chrono::milliseconds);
	
protected:
	//Additional state variable to indicate whether a network entity is online or not
	bool m_isOnline;
	std::chrono::milliseconds m_heartbeatTimeout;
};
#endif 
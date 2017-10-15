//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description	: Base class for a network entity (server or client)
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

#include "networkentity.h"

using namespace std::chrono_literals;

bool INetworkEntity::IsOnline()
{
	return m_isOnline;
}

void INetworkEntity::setHeartbeatTimeout(std::chrono::milliseconds timeout)
{
	m_heartbeatTimeout = timeout;
}

INetworkEntity::INetworkEntity()
	: m_heartbeatTimeout{ 5000ms }
{
}

#include "networkentity.h"

using namespace std::chrono_literals;

bool INetworkEntity::IsOnline()
{
	return m_bOnline;
}

void INetworkEntity::setHeartbeatTimeout(std::chrono::milliseconds timeout)
{
	m_heartbeatTimeout = timeout;
}

INetworkEntity::INetworkEntity()
	: m_heartbeatTimeout{ 5000ms }
{
}

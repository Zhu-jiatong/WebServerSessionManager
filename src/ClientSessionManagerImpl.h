#ifndef CLIENT_SESSION_MANAGER_IMPL_h
#define CLIENT_SESSION_MANAGER_IMPL_h

#include <cstring>
#include <string>
#include <iomanip>
#include "ClientSessionManager.h"

template<typename KeyType>
std::chrono::time_point<std::chrono::high_resolution_clock> SessionBase<KeyType>::m_firstInstantiationTime = std::chrono::high_resolution_clock::now();

template<typename ClientSessionType>
void ClientSessionManager<ClientSessionType>::updateSessions()
{
}

template<typename ClientSessionType>
void ClientSessionManager<ClientSessionType>::createSession(session_type sessionData)
{
	SessionManagerBase<ClientSessionType>::m_fn_storeSession(sessionData);
}

template<typename ClientSessionType>
void ClientSessionManager<ClientSessionType>::terminateSession(key_type sessionId)
{
	SessionManagerBase<ClientSessionType>::m_fn_deleteSession(sessionId);
}

template<typename ClientSessionType>
ClientSessionType ClientSessionManager<ClientSessionType>::getSessionInformation(key_type sessionId)
{
	return SessionManagerBase<ClientSessionType>::m_fn_retrieveSession(sessionId);;
}

template<typename ClientSessionType>
std::string ClientSessionManager<ClientSessionType>::sessionIdToString(key_type sessionId)
{
	std::ostringstream ss;
	for (size_t i = 0; i < 32; ++i)
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(sessionId[i]);
	return ss.str();
}

template<typename ClientSessionType>
typename ClientSessionManager<ClientSessionType>::key_type ClientSessionManager<ClientSessionType>::sessionIdToArray(std::string sessionIdString)
{
	std::shared_ptr<uint8_t[]> output(new uint8_t[32]);
	for (size_t i = 0; i < sessionIdString.length(); i += 2)
	{
		std::string byteString = sessionIdString.substr(i, 2);
		output[i / 2] = std::stoi(byteString, nullptr, 16);
	}
	return output;
}

#endif // !CLIENT_SESSION_MANAGER_IMPL_h

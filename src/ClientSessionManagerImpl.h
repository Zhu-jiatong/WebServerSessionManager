#ifndef CLIENT_SESSION_MANAGER_IMPL_h
#define CLIENT_SESSION_MANAGER_IMPL_h

#include <sstream>
#include <cstring>
#include <string>
#include <mbedtls/sha256.h>
#include <iostream>
#include <iomanip>
#include "ClientSessionManager.h"

template<typename KeyType>
std::chrono::time_point<std::chrono::high_resolution_clock> SessionBase<KeyType>::m_firstInstantiationTime = std::chrono::high_resolution_clock::now();

ClientSession::ClientSession(const std::string& userId, uint32_t clientIp) :
	SessionBase(userId, clientIp)
{
	m_sessionId = generateId();
}

ClientSession::ClientSession(const std::string& userId, uint32_t clientIp, key_type sessionId) :
	SessionBase(userId, clientIp)
{
	m_sessionId = sessionId;
}

ClientSession::key_type ClientSession::generateId()
{
	std::shared_ptr<uint8_t[32]> sessionId(new uint8_t[32], std::default_delete<uint8_t[]>());
	auto period = std::chrono::duration_cast<std::chrono::microseconds>(m_instantiationTimestamp - m_firstInstantiationTime).count();
	std::cout << period << '\n';
	std::ostringstream ss;
	ss << period << m_clientIP;
	const std::string& inputStr = ss.str();
	const char* inputCstr = inputStr.c_str();

	mbedtls_sha256_ret(reinterpret_cast<const uint8_t*>(inputCstr), strlen(inputCstr), sessionId.get(), 0);

	return sessionId;
}

template<typename ClientSessionType>
void ClientSessionManager<ClientSessionType>::begin()
{
}

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
	std::shared_ptr<uint8_t[32]> output(new uint8_t[32], std::default_delete<uint8_t[]>());
	for (size_t i = 0; i < sessionIdString.length(); i += 2)
	{
		std::string byteString = sessionIdString.substr(i, 2);
		output[i / 2] = std::stoi(byteString, nullptr, 16);
	}
	return output;
}

#endif // !CLIENT_SESSION_MANAGER_IMPL_h

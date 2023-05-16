#pragma once
#include "ClientSessionManagerImpl.h"
#include <sstream>
#include <cstring>
#include <string>
#include <mbedtls/sha256.h>
#include <iostream>
#include <iomanip>

template<typename ClientSessionType>
inline void ClientSessionManager<ClientSessionType>::begin()
{
}

template<typename ClientSessionType>
void ClientSessionManager<ClientSessionType>::updateSessions()
{
}

template<typename ClientSessionType>
void ClientSessionManager<ClientSessionType>::createSession(session_type sessionData)
{
	std::shared_ptr<uint8_t[32]> sessionId = generateId(sessionData.m_clientIP);
	sessionData.m_sessionId = sessionId;
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
std::shared_ptr<uint8_t[32]> ClientSessionManager<ClientSessionType>::sessionIdToArray(std::string sessionIdString)
{
	std::shared_ptr<uint8_t[32]> output(new uint8_t[32], std::default_delete<uint8_t[]>());
	for (size_t i = 0; i < sessionIdString.length(); i += 2)
	{
		std::string byteString = sessionIdString.substr(i, 2);
		output[i / 2] = std::stoi(byteString, nullptr, 16);
	}
	return output;
}

template<typename ClientSessionType>
std::shared_ptr<uint8_t[32]> ClientSessionManager<ClientSessionType>::generateId(uint32_t clientIpAddress)
{
	std::shared_ptr<uint8_t[32]> output(new uint8_t[32], std::default_delete<uint8_t[]>()); // TODO: remove `std::default_delete<uint8_t[]>()` after v3.0.0 (5.1)

	auto timestamp = std::chrono::high_resolution_clock::now();
	auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(timestamp - initialTimestamp).count();

	std::ostringstream ss;
	ss << period << clientIpAddress;
	const std::string& inputStr = ss.str();
	const char* inputCstr = inputStr.c_str();

	mbedtls_sha256_ret(reinterpret_cast<const uint8_t*>(inputCstr), strlen(inputCstr), output.get(), 0);
	return output;
}

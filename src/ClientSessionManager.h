#pragma once
#include "SessionManagerBase.h"
#include <functional>
#include <memory>
#include <type_traits>

struct ClientSession : public Session<uint8_t*>
{
public:
	ClientSession(uint32_t m_clientIP, const char* userId);

	std::shared_ptr<uint8_t[32]> m_sessionId;

private:
	void generateId();
	static std::chrono::time_point<std::chrono::high_resolution_clock> m_firstInstantiationTime;
};

template<typename ClientSessionType = ClientSession>
class ClientSessionManager :
	public SessionManagerBase<ClientSessionType>
{
public:
	using SessionManagerBase<ClientSessionType>::SessionManagerBase;
	using session_type = typename SessionManagerBase<ClientSessionType>::session_type;
	using key_type = typename SessionManagerBase<ClientSessionType>::key_type;

	void begin();
	void updateSessions();
	void createSession(session_type sessionData);
	void terminateSession(key_type sessionId);
	ClientSessionType getSessionInformation(key_type sessionId); // TODO: use `std::optional`

	static std::string sessionIdToString(key_type sessionId);
	static std::shared_ptr<uint8_t[32]> sessionIdToArray(std::string sessionIdString);
};

#include "ClientSessionManagerImpl.h"

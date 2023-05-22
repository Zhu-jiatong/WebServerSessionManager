#pragma once
#include "SessionManagerBase.h"
#include <functional>
#include <memory>
#include <type_traits>

struct ClientSession : public SessionBase<std::shared_ptr<uint8_t[32]>>
{
public:
	ClientSession(const std::string& userId, uint32_t clientIp);
	ClientSession(const std::string& userId, uint32_t clientIp, key_type sessionId);

private:
	key_type generateId();
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
	static key_type sessionIdToArray(std::string sessionIdString);
};

#include "ClientSessionManagerImpl.h"

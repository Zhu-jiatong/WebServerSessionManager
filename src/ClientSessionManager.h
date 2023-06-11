#ifndef CLIENT_SESSION_MANAGER_h
#define CLIENT_SESSION_MANAGER_h

#include "SessionManagerBase.h"
#include <functional>
#include <memory>
#include <mbedtls/sha256.h>
#include <sstream>

struct ClientSession : public SessionBase<std::shared_ptr<uint8_t[]>>
{
public:
	ClientSession(const std::string& userId, uint32_t clientIp) :
		SessionBase(userId, clientIp)
	{
		m_sessionId = generateId();
	}
	ClientSession(const std::string& userId, uint32_t clientIp, key_type sessionId) :
		SessionBase(userId, clientIp)
	{
		m_sessionId = sessionId;
	}

private:
	key_type generateId()
	{
		std::shared_ptr<uint8_t[]> sessionId(new uint8_t[32]);
		auto period = std::chrono::duration_cast<std::chrono::microseconds>(m_instantiationTimestamp - m_firstInstantiationTime).count();

		std::ostringstream ss;
		ss << period << m_clientIP;
		const std::string& inputStr = ss.str();
		const char* inputCstr = inputStr.c_str();

		mbedtls_sha256_ret(reinterpret_cast<const uint8_t*>(inputCstr), strlen(inputCstr), sessionId.get(), 0);

		return sessionId;
	}
};

template<typename ClientSessionType = ClientSession>
class ClientSessionManager :
	public SessionManagerBase<ClientSessionType>
{
public:
	using SessionManagerBase<ClientSessionType>::SessionManagerBase;
	using session_type = typename SessionManagerBase<ClientSessionType>::session_type;
	using key_type = typename SessionManagerBase<ClientSessionType>::key_type;

	void updateSessions();
	void createSession(session_type sessionData);
	void terminateSession(key_type sessionId);
	ClientSessionType getSessionInformation(key_type sessionId); // TODO: use `std::optional`

	static std::string sessionIdToString(key_type sessionId);
	static key_type sessionIdToArray(std::string sessionIdString);
};

#include "ClientSessionManagerImpl.h"

#endif // !CLIENT_SESSION_MANAGER_h

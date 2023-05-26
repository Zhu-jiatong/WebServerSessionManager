#ifndef SESSION_MANAGER_BASE_h
#define SESSION_MANAGER_BASE_h

#include <chrono>
#include <functional>

template<typename KeyType>
struct SessionBase
{
public:
	using key_type = KeyType;

	SessionBase(const std::string& userId, uint32_t clientIP) :
		m_userId(userId),
		m_clientIP(clientIP),
		m_instantiationTimestamp(std::chrono::high_resolution_clock::now()) {};

	std::string m_userId;
	uint32_t m_clientIP;
	key_type m_sessionId;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_instantiationTimestamp;
	static std::chrono::time_point<std::chrono::high_resolution_clock> m_firstInstantiationTime;

protected:
	virtual key_type generateId() = 0;
};

template<typename SessionType, typename KeyType = typename SessionType::key_type>
class SessionManagerBase
{
public:
	using session_type = SessionType;
	using key_type = KeyType;

	SessionManagerBase(
		std::function<void()> fn_initialisation,
		std::function<void(session_type)> fn_storeSession,
		std::function<void(key_type)> fn_deleteSession,
		std::function<session_type(key_type)> fn_retrieveSession
	) :
		m_fn_initialisation(fn_initialisation),
		m_fn_storeSession(fn_storeSession),
		m_fn_deleteSession(fn_deleteSession),
		m_fn_retrieveSession(fn_retrieveSession) {}

	virtual void begin() { m_fn_initialisation(); };
	virtual void createSession(session_type) = 0;
	virtual void terminateSession(key_type) = 0;
	virtual void updateSessions() = 0;
	virtual session_type getSessionInformation(key_type) = 0;

protected:
	std::function<void()>m_fn_initialisation;
	std::function<void(session_type)> m_fn_storeSession;
	std::function<void(key_type)> m_fn_deleteSession;
	std::function<session_type(key_type)> m_fn_retrieveSession;
};

#endif // !SESSION_MANAGER_BASE_h

#pragma once

#include <chrono>
#include <functional>

template<typename KeyType>
struct Session
{
public:
	using key_type = KeyType;

	Session(const char* userId, uint32_t clientIP) :
		m_userId(userId),
		m_clientIP(clientIP),
		m_lastActiveTimestamp(std::chrono::high_resolution_clock::now()) {};

	const char* m_userId;
	uint32_t m_clientIP;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastActiveTimestamp;

};

template<typename SessionType, typename KeyType = typename SessionType::key_type>
class SessionManagerBase
{
public:
	using session_type = SessionType;
	using key_type = KeyType;

	SessionManagerBase(
		std::function<void(session_type)> fn_storeSession,
		std::function<void(key_type)> fn_deleteSession,
		std::function<session_type(key_type)> fn_retrieveSession
	) :
		m_fn_storeSession(fn_storeSession),
		m_fn_deleteSession(fn_deleteSession),
		m_fn_retrieveSession(fn_retrieveSession)
	{}

	virtual void begin() = 0;
	virtual void createSession(session_type) = 0;
	virtual void terminateSession(key_type) = 0;
	virtual void updateSessions() = 0;
	virtual session_type getSessionInformation(key_type) = 0;

protected:
	std::function<void(session_type)> m_fn_storeSession;
	std::function<void(key_type)> m_fn_deleteSession;
	std::function<session_type(key_type)> m_fn_retrieveSession;
};

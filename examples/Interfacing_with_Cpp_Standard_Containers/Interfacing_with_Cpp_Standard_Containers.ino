/*
 Name:		Interfacing_with_Cpp_Standard_Containers.ino
 Created:	5/17/2023 1:03:57 AM
 Author:	jiaji
*/

#include "ClientSessionManager.h"
#include <iostream>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <string>

using SessionManager = ClientSessionManager<>;
using SessionType = SessionManager::session_type;
using KeyType = SessionType::key_type;

std::unordered_map<std::string, SessionType> sessions;

void storeSession(SessionType sessionData);
void deleteSession(KeyType sessionId);
SessionType retrieveSession(KeyType sessionId);

SessionManager sessionManager(storeSession, deleteSession, retrieveSession);

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	std::cout << "Starting test\n";

	sessionManager.begin();

	SessionType session1(12345, "Test User 1");
	SessionType session2(67890, "Test User 2");
	SessionType session3(13579, "Test User 3");

	sessionManager.createSession(session1);
	sessionManager.createSession(session2);
	sessionManager.createSession(session3);

	std::cout << "Setup complete, ready to accept user input\n";
}

// the loop function runs over and over again until power down or reset
void loop()
{

}

void serialEvent()
{
	std::string hashString;

	while (Serial.available())
		hashString += static_cast<char>(Serial.read());

	auto querry = SessionManager::sessionIdToArray(hashString);

	std::string sessionId = SessionManager::sessionIdToString(querry.get());
	SessionType session = sessionManager.getSessionInformation(querry.get());
	std::cout << session.m_userId << '\n';
}

void storeSession(SessionType sessionData)
{
	std::string sessionId = SessionManager::sessionIdToString(sessionData.m_sessionId.get());
	std::cout << "Stored Session: \"" << sessionData.m_userId << "\" with ID: \"" << sessionId << "\"\n";
	sessions.emplace(sessionId, sessionData);
}

void deleteSession(KeyType sessionId)
{
	std::string key = SessionManager::sessionIdToString(sessionId);
	std::cout << "Deleting session: " << '\n';
	sessions.erase(key);
}

SessionType retrieveSession(KeyType sessionId)
{
	SessionType& thisSession = sessions.at(SessionManager::sessionIdToString(sessionId));
	return thisSession;
}
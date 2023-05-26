/*
 Name:		Interfacing_with_SQLite.ino
 Created:	5/26/2023 8:07:53 AM
 Author:	jiaji
*/

#include "ClientSessionManager.h"
#include <sqlite3.h>
#include <SD.h>
#include <string>
#include <iostream>

using SessionManager = ClientSessionManager<>;
using KeyType = SessionManager::key_type;
using SessionType = SessionManager::session_type;

void sessionManagerInitialisation();
void storeSession(SessionType sessionData);
void deleteSession(KeyType sessionId);
SessionType retrieveSession(KeyType sessionId);

SessionManager sessionManager(sessionManagerInitialisation, storeSession, deleteSession, retrieveSession);

const std::string SESSION_DATABASE_FILE = "/sd/session.db";

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	SD.begin();

	sessionManager.begin();

	sessionManager.createSession({ "Test User 1", 123456 });
	sessionManager.createSession({ "Test User 2", 67890 });
	sessionManager.createSession({ "Test User 3", 13579 });
}

// the loop function runs over and over again until power down or reset
void loop() {

}

void serialEvent()
{
	std::string hashString;

	while (Serial.available())
		hashString += static_cast<char>(Serial.read());

	auto querry = SessionManager::sessionIdToArray(hashString);

	SessionType session = sessionManager.getSessionInformation(querry);
	std::cout << "Username: " << session.m_userId << '\n';
}

void sessionManagerInitialisation()
{
	if (SD.exists("/session.db"))
	{
		SD.remove("/session.db");
	}

	sqlite3* db;
	if (sqlite3_open(SESSION_DATABASE_FILE.c_str(), &db) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_close(db);
		return;
	}

	const char* querry = "CREATE TABLE IF NOT EXISTS sessionData (sessionId char(64) NOT NULL PRIMARY KEY, username text NOT NULL, ip int NOT NULL)";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, querry, -1, &stmt, nullptr) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

void storeSession(SessionType sessionData)
{
	sqlite3* db;
	if (sqlite3_open(SESSION_DATABASE_FILE.c_str(), &db) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_close(db);
		return;
	}

	sqlite3_stmt* stmt;
	const char* querry = "INSERT INTO sessionData VALUES (?, ?, ?)";
	if (sqlite3_prepare_v2(db, querry, -1, &stmt, nullptr) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	std::string sessionId = SessionManager::sessionIdToString(sessionData.m_sessionId);
	if (sqlite3_bind_text(stmt, 1, sessionId.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	if (sqlite3_bind_text(stmt, 2, sessionData.m_userId.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}


	if (sqlite3_bind_int(stmt, 3, sessionData.m_clientIP) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	std::cout << "Stored Session: \"" << sessionData.m_userId << "\" with ID \"" << sessionId << "\"\n";
}

void deleteSession(KeyType sessionId)
{
	sqlite3* db;
	sqlite3_open(SESSION_DATABASE_FILE.c_str(), &db);
	sqlite3_stmt* stmt;
	const char* querry = "DELETE FROM sessionData WHERE sessionId=?";
	sqlite3_prepare_v2(db, querry, -1, &stmt, nullptr);

	std::string sessionIdStr = SessionManager::sessionIdToString(sessionId);
	sqlite3_bind_text(stmt, 1, sessionIdStr.c_str(), -1, SQLITE_STATIC);

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

SessionType retrieveSession(KeyType sessionId)
{
	sqlite3* db;
	if (sqlite3_open(SESSION_DATABASE_FILE.c_str(), &db) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_close(db);
		exit(1);
	}

	sqlite3_stmt* stmt;
	const char* querry = "SELECT username, ip FROM sessionData WHERE sessionId=?";
	if (sqlite3_prepare_v2(db, querry, -1, &stmt, nullptr) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		exit(1);
	}

	std::string sessionIdStr = SessionManager::sessionIdToString(sessionId);
	if (sqlite3_bind_text(stmt, 1, sessionIdStr.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		exit(1);
	}

	if (sqlite3_step(stmt) != SQLITE_ROW)
	{
		std::cout << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		exit(1);
	}

	std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
	int ip = sqlite3_column_int(stmt, 1);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return SessionType(username, ip, sessionId);
}
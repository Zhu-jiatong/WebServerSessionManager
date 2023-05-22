#ifndef SERVER_SESSION_MANAGER_h
#define SERVER_SESSION_MANAGER_h

#include "SessionManagerBase.h"
#include <lwip/etharp.h>

struct ServerSession : SessionBase<uint32_t>
{
public:
	using SessionBase::SessionBase;

private:
	void getMac();

	eth_addr m_mac;
};

#endif // !SERVER_SESSION_MANAGER_h

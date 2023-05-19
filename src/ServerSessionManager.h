#pragma once

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
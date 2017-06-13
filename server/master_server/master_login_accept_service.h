#pragma once

#include "base/types.h"
#include <queue>
#include "srv/net/server_accept_service.h"
#include "comm/proto/server_server.pb.h"

namespace terra
{
	//login server object
	//合并成一个server manager?
	class MasterLoginAcceptService : public ServerAcceptService
	{
		MAKE_INSTANCE(MasterLoginAcceptService);
		DISABLE_COPY(MasterLoginAcceptService);
	public:
		MasterLoginAcceptService();
		~MasterLoginAcceptService() = default;
	private:
	};
}
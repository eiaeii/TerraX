#pragma once
#include "NetDefine.h"
#include "ComDef.h"
#include "EventLoop.h"
#include "IServer.h"
namespace TerraX
{
	const int MAX_CONNECTION = 1024;
	class GateServer final : public IServer
	{
		NOCOPY(GateServer);
		MAKEINSTANCE(GateServer);
	public:
		GateServer();
		~GateServer() = default;

		bool Init(/*Config Info*/) override;
		void Run() override;
		void Exit() override { m_bExit = true; }

		//FrontEnd* GetFrontEnd() { return m_pFrontEnd.get(); }
		//BackEnd* GetBackEnd(){ return m_pBackEnd.get(); }
	protected:
		bool InitStaticModule() override;
		bool InitNetModule() override;

		void ProcessLogic() override;

	private:
		bool m_bExit{ false };
	};
}
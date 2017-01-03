#include <chrono>
#include <thread>
#include "GateServer.h"
#include "PacketProcessor_Gate.h"
#include "ServerManager.h"
#include "GateLoginManager.h"
#include "TimeManager.h"
using namespace TerraX;

GateServer::GateServer() {}

bool GateServer::InitStaticModule()
{
    PacketProcessor_Gate::GetInstance();
	ServerManager::GetInstance();
	GateLoginManager::GetInstance();
	TimeManager::GetInstance();
    return true;
}

bool GateServer::InitNetModule()
{
    PacketProcessor_Gate::GetInstance().Connect("127.0.0.1", 9995);
    PacketProcessor_Gate::GetInstance().Accept(9991, MAX_CONNECTION);

    return true;
}

bool GateServer::Init()
{
    InitStaticModule();
    InitNetModule();
    return true;
}

void GateServer::Run()
{
    while (!m_bExit) {
		TimeManager::GetInstance().Start();

        ProcessLogic();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

		auto costms = TimeManager::GetInstance().FrameTimePoint();
        if (costms < 50) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50 - costms));
        }
        // std::cout << std::this_thread::get_id() << ": " << costms << std::endl;
    }
}

void GateServer::ProcessLogic() { PacketProcessor_Gate::GetInstance().Tick(); }

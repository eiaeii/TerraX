#include "login_net_module.h"

#include "comm/config/server_config.h"
#include "comm/net/packet_dispatcher.h"

using namespace terra;
using namespace packet_ss;

//TODO ���ݰ���������ΪInstance()
LoginNetModule::LoginNetModule() : NetBaseModule(PeerType_t::GATESERVER),
conn_service_(ServerConnService::GetInstance())
{
	conn_service_.InitNetModule(this);
}

void LoginNetModule::InitLoginNetInfo()
{
	ServerConfig::GetInstance().LoadConfigFromJson("login_server.json");

    std::string conn_ip;
    int conn_port;
    ServerConfig::GetInstance().GetJsonObjectValue("net", "master_server_ip", conn_ip);
    ServerConfig::GetInstance().GetJsonObjectValue("net", "master_server_port", conn_port);
    InitConnectInfo(conn_ip, conn_port);

    std::string listen_ip;
    int listen_port;
    ServerConfig::GetInstance().GetJsonObjectValue("net", "listen_ip", listen_ip);
    ServerConfig::GetInstance().GetJsonObjectValue("net", "listen_port", listen_port);
    InitListenInfo(listen_ip, listen_port);
}

void LoginNetModule::StartConnectMasterServer()
{
    TcpConnection* conn = conn_service_.NewConnect(
        conn_ip_.c_str(), conn_port_,
        [this](TcpConnection* conn, ConnState_t conn_state) { this->OnServerSocketEvent(conn, conn_state); },
        [this](TcpConnection* conn, evbuffer* evbuf) { this->OnServerMessageEvent(conn, evbuf); });
	server_table_.AddServerInfo(PeerType_t::MASTERSERVER, 0, conn_ip_.c_str(),
                                             conn_port_, conn);
}

void LoginNetModule::StartAcceptClient()
{
	accpet_service_.AcceptConnection(get_listen_port(), 1024, 
		[this](TcpConnection* conn, ConnState_t conn_state) { this->OnClientSocketEvent(conn, conn_state); },
		[this](TcpConnection* conn, evbuffer* evbuf) { this->OnClientMessageEvent(conn, evbuf); }););
}

bool LoginNetModule::Init()
{
    CONSOLE_DEBUG_LOG(LEVEL_INFO, "Gate Server Start...");
    InitLoginNetInfo();
    StartConnectMasterServer();
	StartAcceptClient();
    return true;
}
bool LoginNetModule::AfterInit() { return true; }
bool LoginNetModule::Tick()
{
    get_event_loop()->loop();
    return true;
}
bool LoginNetModule::BeforeShut() { return true; }
bool LoginNetModule::Shut() { return true; }

void LoginNetModule::OnServerSocketEvent(TcpConnection* conn, ConnState_t conn_state)
{
    switch (conn_state) {
        case ConnState_t::CONNECTED: {
			OnMasterConnected(conn);
        } break;
        case ConnState_t::DISCONNECTED: {
			OnMasterDisconnected(conn);
			//server_table_.PrintServerTable();
        } break;
        default:
            break;
    }
}
void LoginNetModule::OnServerMessageEvent(TcpConnection* conn, evbuffer* evbuf)
{
    ProcessServerMessage(conn, evbuf);
}

void LoginNetModule::OnMasterConnected(TcpConnection* conn)
{
    
};
void LoginNetModule::OnMasterDisconnected(TcpConnection* conn)
{
	server_table_.RemoveByConn(conn);
	conn_service_.DestroyConnection(conn);
    // ReConnect();
}

void LoginNetModule::OnClientConnected(TcpConnection* conn) 
{
}
void LoginNetModule::OnClientDisconnected(TcpConnection* conn)
{
	server_table_.RemoveByConn(conn);
}
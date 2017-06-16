#include "node_net_module.h"

#include "comm/config/server_config.h"

using namespace terra;

NodeNetModule::NodeNetModule()
    : NetBaseModule(PeerType_t::NODESERVER),
      conn_service_(NodeConnService::GetInstance()),
      accept_service_(NodeAcceptService::GetInstance())
{
	conn_service_.InitNetModule(this);
	accept_service_.InitNetModule(this);
}

void NodeNetModule::InitNodeNetInfo()
{
    ServerConfig::GetInstance().LoadConfigFromJson("node_server.json");

    std::string conn_ip;
    int conn_port;
    ServerConfig::GetInstance().GetJsonObjectValue("net", "world_server_ip", conn_ip);
    ServerConfig::GetInstance().GetJsonObjectValue("net", "world_server_port", conn_port);
    InitConnectInfo(conn_ip, conn_port);

    std::string listen_ip;
    int listen_port;
    ServerConfig::GetInstance().GetJsonObjectValue("net", "listen_ip", listen_ip);
    ServerConfig::GetInstance().GetJsonObjectValue("net", "listen_port", listen_port);
    InitListenInfo(listen_ip, listen_port);
}

void NodeNetModule::StartConnectWorldServer()
{
    conn_service_.Connect2World(
        conn_ip_.c_str(), conn_port_,
        [this](TcpConnection* conn, SocketEvent_t ev) { this->OnWorldSocketEvent(conn, ev); },
        [this](TcpConnection* conn, evbuffer* evbuf) { this->OnWorldMessageEvent(conn, evbuf); });
}

void NodeNetModule::StartAccept()
{
    accept_service_.AcceptConnection(
        get_listen_port(), 64,
        [this](TcpConnection* conn, SocketEvent_t ev) { this->OnGateSocketEvent(conn, ev); },
        [this](TcpConnection* conn, evbuffer* evbuf) { this->OnGateMessageEvent(conn, evbuf); });
}

bool NodeNetModule::Init()
{
    CONSOLE_DEBUG_LOG(LEVEL_INFO, "Node Server Start...");
    InitNodeNetInfo();
    StartConnectWorldServer();
    StartAccept();
    return true;
}
bool NodeNetModule::AfterInit() { return true; }
bool NodeNetModule::Tick()
{
    get_event_loop()->loop();
    return true;
}
bool NodeNetModule::BeforeShut() { return true; }
bool NodeNetModule::Shut() { return true; }

void NodeNetModule::OnWorldSocketEvent(TcpConnection* conn, SocketEvent_t ev)
{
    switch (ev) {
        case SocketEvent_t::CONNECTED: {
            conn_service_.OnWorldConnected(conn);
        } break;
		case SocketEvent_t::CONNECT_ERROR:
        case SocketEvent_t::DISCONNECTED: {
			conn_service_.OnWorldDisconnected(conn);
			//server_table_.PrintServerTable();
        } break;
        default:
            break;
    }
}
void NodeNetModule::OnWorldMessageEvent(TcpConnection* conn, evbuffer* evbuf)
{
    ProcessServerMessage(conn, evbuf);
}

void NodeNetModule::OnGateSocketEvent(TcpConnection* conn, SocketEvent_t ev)
{
    switch (ev) {
        case SocketEvent_t::CONNECTED: {
			accept_service_.OnGateConnected(conn);
		} break;
		case SocketEvent_t::CONNECT_ERROR:
        case SocketEvent_t::DISCONNECTED: {
			accept_service_.OnGateDisconnected(conn);
			//server_table_.PrintServerTable();
        } break;
        default:
            break;
    }
}

void NodeNetModule::OnGateMessageEvent(TcpConnection* conn, evbuffer* evbuf)
{
    ProcessServerMessage(conn, evbuf);
}

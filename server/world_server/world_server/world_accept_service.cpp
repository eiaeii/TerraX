#include "world_accept_service.h"
#include "comm/net/packet_dispatcher.h"

using namespace terra;
using namespace packet_ss;

WorldAcceptService::WorldAcceptService(NetBaseModule& net, int aceept_max_conns) 
	: ServerAcceptService(net),
	max_conns_(aceept_max_conns)
{
	for (int i = 1; i < max_conns_; i++) {
		server_ids_.push(i);
	}

	REG_PACKET_HANDLER_ARG3(MsgRegisterSW, this, OnMessage_RegisterSW);
}

void WorldAcceptService::OnLogout(TcpConnection* conn)
{
	NetObject* net_object = server_table_.GetNetObjectByConn(conn);
	assert(net_object);
	server_ids_.push(net_object->server_id_);
} 

void WorldAcceptService::OnMessage_RegisterSW(TcpConnection* conn, int32_t avatar_id, MsgRegisterSW* msg)
{
	assert(server_ids_.size() > 0);

	int server_id = server_ids_.front();
	server_ids_.pop();
	int peer_type = msg->peer_type();

	CONSOLE_DEBUG_LOG(LEVEL_INFO, "%s:\t %d", NetHelper::ServerName(PeerType_t(peer_type)), server_id);

	MsgRegisterWS msgWS;
	msgWS.set_server_id(server_id);
	packet_processor_.SendPacket(conn, msgWS);

	server_table_.AddServerInfo(static_cast<PeerType_t>(peer_type), server_id, msg->listen_ip().c_str(), msg->listen_port(), conn);
}
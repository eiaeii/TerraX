#pragma once

#include "base/types.h"
#include "srv/net/net_base_module.h"
#include "node_conn_service.h"
#include "node_accept_service.h"

namespace terra
{
	class ServerConnService;
	class NodeNetModule final : public NetBaseModule
	{
		DISABLE_COPY(NodeNetModule);
		MAKE_INSTANCE(NodeNetModule);
	private:
		NodeConnService& conn_service_;
		NodeAcceptService& accept_service_;
		std::string world_conn_ip_;
		int world_conn_port_{ 0 };
		std::string gate_listen_ip_;
		int gate_listen_port_{ 0 };
	public:
		NodeNetModule();
		~NodeNetModule() = default;

		const char* get_world_conn_ip() const { return world_conn_ip_.c_str(); }
		int get_world_conn_port()  const { return world_conn_port_; }
		const char* get_gate_listen_ip() const { return gate_listen_ip_.c_str(); }
		int get_gate_listen_port()  const { return gate_listen_port_; }

		bool Init();
		bool AfterInit();
		bool Tick();
		bool BeforeShut();
		bool Shut();
	private:
		void InitNodeNetInfo();
		void StartConnectWorldServer();
		void StartAccept();
		//void AcceptClient() {};
		//void OnClientSocketEvent(TcpConnection* conn, SocketEvent_t ev) {};
		//void OnClientMessage(TcpConnection* conn, evbuffer* evbuf) {};

		void OnWorldSocketEvent(TcpConnection* conn, SocketEvent_t ev);
		void OnWorldMessageEvent(TcpConnection* conn, evbuffer* evbuf);

		void OnGateSocketEvent(TcpConnection* conn, SocketEvent_t ev);
		void OnGateMessageEvent(TcpConnection* conn, evbuffer* evbuf);
	};
}
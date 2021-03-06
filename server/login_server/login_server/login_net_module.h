#pragma once

#include "base/types.h"
#include "srv/net/net_base_module.h"
#include "login_conn_service.h"
#include "login_accept_service.h"
#include "comm/proto/server_server.pb.h"

namespace terra
{
	class ServerConnService;
	class LoginNetModule final : public NetBaseModule
	{
		DISABLE_COPY(LoginNetModule);
		MAKE_INSTANCE(LoginNetModule);
	private:
		LoginConnService& conn_service_;
		LoginAcceptService& accpet_service_;

		std::string master_conn_ip_;
		int master_conn_port_{ 0 };
		std::string client_listen_ip_;
		int client_listen_port_{ 0 };
	public:
		LoginNetModule();
		~LoginNetModule() = default;

		bool Init();
		bool AfterInit();
		bool Tick();
		bool BeforeShut();
		bool Shut();

		int GetLoginServerId() const { return conn_service_.get_login_server_id(); }

		//TODO:
		//		GateAccount, GateAvatar
		//		�����߼� world->gate->node
		void SendPacket2Master(google::protobuf::Message& msg);
		void SendPacket2Client(TcpConnection* conn, google::protobuf::Message& msg);
		void SendPacket2Client(const std::string& account_name, google::protobuf::Message& msg);

		void CloseClientConnection(int fd);
	private:
		void InitLoginNetInfo();
		void StartConnectMasterServer();
		void StartAcceptClient();
		//void OnClientSocketEvent(TcpConnection* conn, SocketEvent_t ev) {};
		//void OnClientMessage(TcpConnection* conn, evbuffer* evbuf) {};

		void OnMasterSocketEvent(TcpConnection* conn, SocketEvent_t ev);
		void OnMasterMessageEvent(TcpConnection* conn, evbuffer* evbuf);

		void OnClientSocketEvent(TcpConnection* conn, SocketEvent_t ev);
		void OnClientMessageEvent(TcpConnection* conn, evbuffer* evbuf);

	};
}
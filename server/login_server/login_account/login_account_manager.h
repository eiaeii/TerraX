#pragma once
#include "base/types.h"
#include "comm/proto/client_server.pb.h"
#include "comm/proto/server_server.pb.h"
#include "srv/net/server_accept_service.h"
#include "login_account_state.h"
#include <unordered_map>
#include <memory>
#include <array>

namespace terra
{
	class LoginAccount;
	class LoginAccountManager
	{
		MAKE_INSTANCE(LoginAccountManager);
		DISABLE_COPY(LoginAccountManager);
		using LoginAccountMap = std::unordered_map<int, std::unique_ptr<LoginAccount> >;
		using Account2FdMap = std::unordered_map<std::string, int >;
		using AccountStateArray = std::array<std::unique_ptr<AccountState_Base>, static_cast<int>(Account_State_t::ACCOUNT_COUNT)>;
	private:
		LoginAccountMap account_map_;
		Account2FdMap account2fd_map_;

		AccountStateArray states_;
	public:
		LoginAccountManager();
		~LoginAccountManager() = default;

		void CreateAccount(TcpConnection* conn);
		void RemoveAccount(TcpConnection* conn);

		void AddAccount2FdInfo(const std::string& account_name, int fd)
		{
			account2fd_map_.insert(std::make_pair(account_name, fd));
		}
		void RemoveAccount2FdInfo(const std::string& account_name)
		{
			account2fd_map_.erase(account_name);
		}

		AccountState_Base* GetAccountState(Account_State_t account_state) { return states_[static_cast<int>(account_state)].get(); }
		LoginAccount* GetAccountByAccountName(const std::string& account_name);
	private:
		void InitAccountState();

		void OnMessage_ReqLoginCL(TcpConnection* conn, int32_t avatar_id, packet_cs::MsgReqLoginCL* msg);
		void OnMessage_ServerListML(packet_ss::MsgServerListML* msg);
		void OnMessage_SelectServerCL(TcpConnection* conn, int32_t avatar_id, packet_cs::MsgSelectServerCL* msg);
		void OnMessage_ReqEnterServerResultSL(packet_ss::MsgReqEnterServerResultSL* msg);
		void OnMessage_QuitLoginCL(TcpConnection* conn, int32_t avatar_id, packet_cs::MsgQuitLoginCL* msg);
	};
}
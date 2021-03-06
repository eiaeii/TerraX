#include "login_account_manager.h"
#include "comm/net/packet_dispatcher.h"

using namespace terra;
using namespace packet_cs;
using namespace packet_ss;

LoginAccountManager::LoginAccountManager()
{
	InitAccountState();
	REG_PACKET_HANDLER_ARG3(MsgReqLoginCL, this, OnMessage_ReqLoginCL); 
	REG_PACKET_HANDLER_ARG1(MsgServerListML, this, OnMessage_ServerListML);
	REG_PACKET_HANDLER_ARG3(MsgSelectServerCL, this, OnMessage_SelectServerCL);
	REG_PACKET_HANDLER_ARG1(MsgReqEnterServerResultSL, this, OnMessage_ReqEnterServerResultSL);
	REG_PACKET_HANDLER_ARG3(MsgQuitLoginCL, this, OnMessage_QuitLoginCL);
}

void LoginAccountManager::InitAccountState()
{
	states_[(int)Account_State_t::ACCOUNT_WAITING_LOGIN].reset(new AccountState_WaitingLogin());
	states_[(int)Account_State_t::ACCOUNT_WAITING_BILLINGAUTH].reset(new AccountState_WaitingBillingAuth());
	states_[(int)Account_State_t::ACCOUNT_WAITING_SERVERLIST].reset(new AccountState_WaitingServerList());
	states_[(int)Account_State_t::ACCOUNT_WAITING_REQ_ENTERSERVER].reset(new AccountState_WaitingReqEnterServer());
	states_[(int)Account_State_t::ACCOUNT_WAITING_GETGATEINFO].reset(new AccountState_WaitingGetGateInfo());
	states_[(int)Account_State_t::ACCOUNT_WAITING_CLIENTSWITCH2GATE].reset(new AccountState_WaitingClientSwitch2Gate());
	states_[(int)Account_State_t::ACCOUNT_DESTROY].reset(new AccountState_Destory());
}

void LoginAccountManager::CreateAccount(TcpConnection* conn)
{
	std::unique_ptr<LoginAccount> account(new LoginAccount(conn));
	account->EnterDefaultState();
	accounts_.InsertPKeyValue(conn->get_fd(), std::move(account));
}


void LoginAccountManager::RemoveAccount(TcpConnection* conn)
{
	accounts_.EraseValueByPrimaryKey(conn->get_fd());
}

void LoginAccountManager::AddAccount2FdInfo(const std::string& account_name, int fd)
{
	accounts_.SetFKey2PKey(account_name, fd);
}
void LoginAccountManager::RemoveAccount2FdInfo(const std::string& account_name)
{
	accounts_.EraseForeignKeyOnly(account_name);
}

LoginAccount* LoginAccountManager::GetAccountByAccountName(const std::string& account_name)
{
	auto ptr = accounts_.GetValueByForeignkey(account_name);
	if (!ptr)
	{
		return nullptr;
	}
	return (*ptr).get();
}

void LoginAccountManager::OnMessage_ReqLoginCL(TcpConnection* conn, int32_t avatar_id, MsgReqLoginCL* msg)
{
	ProcessMessageByfd(conn->get_fd(), msg);
}

void LoginAccountManager::OnMessage_ServerListML(MsgServerListML* msg)
{
	ProcessMessageByAccountName(msg->post_back().account_name(), msg);
}

void LoginAccountManager::OnMessage_SelectServerCL(TcpConnection* conn, int32_t avatar_id, packet_cs::MsgSelectServerCL* msg)
{
	ProcessMessageByfd(conn->get_fd(), msg);
}


void LoginAccountManager::OnMessage_ReqEnterServerResultSL(MsgReqEnterServerResultSL* msg)
{
	ProcessMessageByAccountName(msg->account_name(), msg);
}

void LoginAccountManager::OnMessage_QuitLoginCL(TcpConnection* conn, int32_t avatar_id, packet_cs::MsgQuitLoginCL* msg)
{
	ProcessMessageByfd(conn->get_fd(), msg);
}
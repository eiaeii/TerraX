#include "game_state_manager.h"
#include "comm/net/packet_dispatcher.h"
#include "comm/proto/base_type.pb.h"
#include "guest.h"
#include "client_net_module.h"

using namespace terra;
using namespace packet_cs;

GameStateManager::GameStateManager() : m_CurGameState(GameState_t::PRESS2START)
{
	m_GameStates[int(GameState_t::PRESS2START)].reset(new GameState_Press2Start);
	m_GameStates[int(GameState_t::LOGIN_FORM)].reset(new GameState_eLoginForm);
	m_GameStates[int(GameState_t::CONNECTING2LOGIN)].reset(new GameState_Connecting2Login);
	m_GameStates[int(GameState_t::ACCOUNT_LOGGINGIN)].reset(new GameState_AccountLoggingin);
	m_GameStates[int(GameState_t::ACCOUNT_CHOSINGSERVER)].reset(new GameState_ChosingServer);
	m_GameStates[int(GameState_t::CONNECTING_GATE)].reset(new GameState_Connecting2Gate);
	m_GameStates[int(GameState_t::ACCOUNT_CHECKINGPERMISSION)].reset(new GameState_CheckingPermission);
	m_GameStates[int(GameState_t::SELECTING_AVATAR)].reset(new GameState_AccountSelectingAvatar);
	m_GameStates[int(GameState_t::ENTERING_GAME)].reset(new GameState_AccountEnteringWorld);
	m_GameStates[int(GameState_t::ENTERING_SCENE)].reset(new GameState_PlayerEnteringScene);
	m_GameStates[int(GameState_t::GAMING)].reset(new GameState_Gaming);

	REG_PACKET_HANDLER_ARG1(MsgLoginResultLC, this, OnMessage_LoginResultLC);
	REG_PACKET_HANDLER_ARG1(MsgSeclectServerResultLC, this, OnMessage_SeclectServerResultLC);
	//REG_PACKET_HANDLER_ARG1(PktRoleListAck, this, OnMessage_PktRoleListAck);
}

void GameStateManager::Tick() { m_GameStates[int(m_CurGameState)]->Tick(); }

void GameStateManager::NextState(GameState_t eGameState)
{
	m_GameStates[int(m_CurGameState)]->Leave();
	m_CurGameState = eGameState;
	m_GameStates[int(m_CurGameState)]->Enter();
}

void GameStateManager::EnterDefaultState() { m_GameStates[int(GameState_t::PRESS2START)]->Enter(); }

const char kServerStatus[][16] = {
	"维护",
	"畅通",
	"繁忙",
	"拥挤",
	"爆满"
};

void GameStateManager::OnMessage_LoginResultLC(MsgLoginResultLC* msg)
{
	assert(m_CurGameState == GameState_t::ACCOUNT_LOGGINGIN);
	int login_result = msg->result();
	if (login_result == 0) // success
	{
		Guest::GetInstance().set_login_token(msg->token());
		for (int i = 0; i < msg->servers_size(); i++)
		{
			//save server info
			auto& val = msg->servers(i);
			if (val.server_status() == pb_base::MAINTAIN)
			{
				CONSOLE_DEBUG_LOG(LEVEL_DEFAUT, "[%d][%s][%s][%s]",
					val.server_uid(), val.region_name().c_str(), val.server_name().c_str(),
					kServerStatus[val.server_status()]);
			}
			else if (val.server_status() == pb_base::FREE)
			{
				CONSOLE_DEBUG_LOG(LEVEL_INFO, "[%d][%s][%s][%s]",
					val.server_uid(), val.region_name().c_str(), val.server_name().c_str(),
					kServerStatus[val.server_status()]);
			}
			else if (val.server_status() == pb_base::BUSY || val.server_status() == pb_base::CROWD)
			{
				CONSOLE_DEBUG_LOG(LEVEL_WARNING, "[%d][%s][%s][%s]",
					val.server_uid(), val.region_name().c_str(), val.server_name().c_str(),
					kServerStatus[val.server_status()]);
			}
			else
			{
				CONSOLE_DEBUG_LOG(LEVEL_ERROR, "[%d][%s][%s][%s]",
					val.server_uid(), val.region_name().c_str(), val.server_name().c_str(),
					kServerStatus[val.server_status()]);
			}
		}
		GameStateManager::GetInstance().NextState(GameState_t::ACCOUNT_CHOSINGSERVER);
	}
	else
	{
		CONSOLE_DEBUG_LOG(LEVEL_INFO, "login failed! error code: %d", login_result);
	}

}

void GameStateManager::OnMessage_SeclectServerResultLC(MsgSeclectServerResultLC* msg)
{
	if (msg->result() == 0) // success
	{
		ClientNetModule::GetInstance().SetGateIpPort(msg->gate_ip(), msg->gate_port());
		CONSOLE_DEBUG_LOG(LEVEL_INFO, "gate ip: %s    port: %d", msg->gate_ip().c_str(), msg->gate_port());
		GameStateManager::GetInstance().NextState(GameState_t::CONNECTING_GATE);
	}
	else
	{
		CONSOLE_DEBUG_LOG(LEVEL_ERROR, "enter server failed! error code: %d", msg->result());
		//error: io block, login server can not receive quit msg
		//getchar();
		//GameStateManager::GetInstance().NextState(GameState_t::LOGIN_FORM);

		MsgQuitLoginCL req;
		ClientNetModule::GetInstance().SendPacket2LoginServer(req);
	}
}
/*void GameStateManager::OnMessage_PktRoleListAck(PktRoleListAck* msg)
{
	assert(pkt);
	std::cout << "role-list:" << std::endl;
	for (int i = 0; i < pkt->role_name_list_size(); ++i) {
		std::cout << " - " << pkt->role_name_list(i).c_str() << std::endl;
	}
	GameStateManager::GetInstance().NextState(GameState_t::SELECTING_AVATAR);
}
*/
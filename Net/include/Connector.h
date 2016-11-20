#pragma once
#include <memory>
#include "NetDefine.h"
#include "NetChannel.h"
namespace TerraX
{
	template<class T, PeerType_t peertype>
	class Connector
	{
	public:
		Connector(EventLoop* pLoop, const std::string& host, int port);
		~Connector() = default;
		template<class Packet> 
		void SendPacket(Packet& packet); 
		bool IsConnected() { return m_Connector.GetConnState() == ConnState_t::eConnected; }

		void SetPeerInfo(PeerInfo& pi) { m_PeerInfo = pi; }
		const PeerInfo& GetPeerInfo() const { return m_PeerInfo; }
	private:
		void Connected();
		void ConnectFailed();
		void Disconnected();
	
	private:
		NetChannel m_Connector;
		PeerInfo m_PeerInfo{ PeerType_t::undefine };
	};

	template<class T, PeerType_t peertype>
	Connector<T, peertype>::Connector(EventLoop* pLoop, const std::string& host, int port)
		: m_Connector(pLoop, host, port), m_PeerInfo(peertype) {
		m_Connector.RegConnected_Callback(std::bind(&Connector<T, peertype>::Connected, this));
		m_Connector.RegConnectFailed_Callback(std::bind(&Connector<T, peertype>::ConnectFailed, this));
		m_Connector.RegDisconnected_Callback(std::bind(&Connector<T, peertype>::Disconnected, this));
	}

	template<class T, PeerType_t peertype>
	void Connector<T, peertype>::Connected() {
		T::GetInstance().Register(m_PeerInfo);
	}

	template<class T, PeerType_t peertype>
	void Connector<T, peertype>::ConnectFailed() {

	}

	template<class T, PeerType_t peertype>
	void Connector<T, peertype>::Disconnected() {

	}

	template<class T, PeerType_t peertype>
	template<class Packet>
	void Connector<T, peertype>::SendPacket(Packet& packet) {
		m_Connector.SendMsg(0, packet);
	}

}
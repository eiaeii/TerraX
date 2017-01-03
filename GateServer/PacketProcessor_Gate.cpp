#include "GuestManager.h"
#include "PacketProcessor_Gate.h"
#include "ServerManager.h"
#include "proto/client_server.pb.h"
#include "proto/server_server.pb.h"

using namespace S2SPacket;
using namespace TerraX;
PacketProcessor_Gate::PacketProcessor_Gate() : PacketProcessor(PeerType_t::gateserver)
{
    RegPacketHandler_Arg1(PktRegisterAck, std::bind(&PacketProcessor_Gate::OnMessage_PktRegisterAck, this,
                                                    std::placeholders::_1));
}

void PacketProcessor_Gate::SendPacket2Server(int dest_info, int owner_info, gpb::Message& msg)
{
    SendPacket2BackEnd(dest_info, owner_info, msg);
}

void PacketProcessor_Gate::ForwardPacketOnBackEnd(NetChannelPtr& pBackChannel, Packet* pkt)
{
    assert(pkt);
    PeerInfo pi(pkt->GetDesination());
    if (pi.peer_type == m_peer_type) {
        std::string packet_name = pkt->GetPacketName();
        if (pi.channel_index <= 0) {
            if (m_pBackEnd) {
                m_pBackEnd->OnMessage(m_pBackEnd->GetPeerInfo(), pkt->GetOwnerInfo(), packet_name,
                                      pkt->GetPacketMsg(), pkt->GetMsgSize());
            }
        } else {
            auto pChannel = m_pFrontEnd->GetChannel(pi.channel_index);
            if (pChannel) {
                pChannel->OnMessage(pChannel->GetPeerInfo(), pkt->GetOwnerInfo(), packet_name,
                                    pkt->GetPacketMsg(), pkt->GetMsgSize());
            }
        }
    }
    if (pi.peer_type == PeerType_t::client) {
        auto pChannel = m_pFrontEnd->GetChannel(pi.channel_index);
        if (pChannel) {
            pChannel->SendMsg(pkt->GetBuffer(), pkt->Size());
        }
    }
}

void PacketProcessor_Gate::ForwardPacketOnFrontEnd(NetChannelPtr& pFrontChannel, Packet* pkt)
{
    assert(pkt);
    PeerInfo pi(pkt->GetDesination());
    Guest* pGuest = GuestManager::GetInstance().GetGuest(pFrontChannel->GetPeerInfo());
	if (!pGuest)
	{
		assert(0);
		return;
	}
    if (pGuest->GetAttachedAvatarID() == 0) {
        pkt->SetOwner(pGuest->GetGuestID());
    } else {
        pkt->SetOwner(pGuest->GetAttachedAvatarID());
    }
    if (m_peer_type == pi.peer_type) {
        std::string packet_name = pkt->GetPacketName();
        pFrontChannel->OnMessage(pFrontChannel->GetPeerInfo(), pkt->GetOwnerInfo(), packet_name,
                                 pkt->GetPacketMsg(), pkt->GetMsgSize());
    } else {
        pkt->SetDestination(pGuest->GetDestPeerInfo(pi.peer_type));
        assert(m_pBackEnd);
        if (m_pBackEnd) {
            m_pBackEnd->SendMsg(pkt->GetBuffer(), pkt->Size());
        }
    }
}

void PacketProcessor_Gate::DoBackEnd_Connected(NetChannelPtr& pChannel) { Login2Center(); }

void PacketProcessor_Gate::DoBackEnd_Disconnected(NetChannelPtr& pChannel) { m_pBackEnd.reset(); }

void PacketProcessor_Gate::DoBackEnd_ConnBreak(NetChannelPtr& pChannel) { m_pBackEnd.reset(); }

void PacketProcessor_Gate::DoFrontEnd_Connected(NetChannelPtr& pChannel)
{
    if (!m_pBackEnd) {
        // kick out client
        return;
    }
    pChannel->SetPeerType(PeerType_t::gateserver);
	pChannel->SetPeerIndex(m_pBackEnd->GetPeerIndex());
	GuestManager::GetInstance().CreateGuest(pChannel->GetPeerInfo());
}
void PacketProcessor_Gate::DoFrontEnd_Disconnected(NetChannelPtr& pChannel)
{
    // RemoveGuest();

    m_pFrontEnd->RemoveChannel(pChannel);
}
void PacketProcessor_Gate::DoFrontEnd_ConnBreak(NetChannelPtr& pChannel)
{
    // RemoveGuest();
    m_pFrontEnd->RemoveChannel(pChannel);
}

void PacketProcessor_Gate::Login2Center()
{
    PeerInfo pi_dest(PeerType_t::centerserver);
    PeerInfo pi_src(PeerType_t::gateserver);
    PktRegisterReq pkt;
    SendPacket2Server(pi_dest.serialize(), pi_src.serialize(), pkt);
}

void PacketProcessor_Gate::OnMessage_PktRegisterAck(PktRegisterAck* pkt)
{
    PeerInfo pi(pkt->server_info());
    assert(pi.peer_type == PeerType_t::gateserver);
    assert(pi.peer_index > 0);
    assert(pi.channel_index == 0 && m_pBackEnd->GetChannelIndex() == 0);
    std::cout << "Server: " << pi.server_name() << "\t PeerIndex: " << int32_t(pi.peer_index)
              << "\t ChannelIndex: " << pi.channel_index << std::endl;
    m_pBackEnd->SetPeerInfo(pkt->server_info());

    // m_pBackEnd.reset();
}
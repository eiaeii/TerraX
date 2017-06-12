#include "packet_processor.h"

#include <event2/buffer.h>
#include <functional>
#include "comm/net/packet_template.h"
#include "comm/net/packet_dispatcher.h"
#include "net_base_module.h"

using namespace terra;

enum class MessageError_t {
    eNoError,
    eInvalidLength,
};

PacketProcessor::PacketProcessor() {}

void PacketProcessor::SendPacket(TcpConnection* conn, google::protobuf::Message& msg)
{
    assert(conn);
    if (!conn) {
        return;
    }
    PacketT<NullTag_t, MsgData> pkt;
    pkt.InitialWithMsg(msg);
    assert(net_object->conn_);
	conn->SendMsg(pkt.get_buffer(), pkt.get_total_len());
}

void PacketProcessor::ProcessServerPacket(TcpConnection* conn, evbuffer* evbuf)
{
    std::size_t readable = evbuffer_get_length(evbuf);
    int32_t min_msg_length = PacketT<NullTag_t, MsgData>::MIN_PACKET_SIZE;  // checksum
    // MessageError_t err = MessageError_t::eNoError;
    while (readable >= static_cast<std::size_t>(min_msg_length)) {
        uint16_t be16 = 0;
        evbuffer_copyout(evbuf, &be16, sizeof(be16));
        uint16_t total_len = ntohs(be16);
        if (total_len > MAX_PACKET_SIZE || total_len < min_msg_length) {
            // err = MessageError_t::eInvalidLength;
            break;
        } else if (readable >= static_cast<std::size_t>(total_len)) {
            PacketT<NullTag_t, MsgData> pkt;
            evbuffer_remove(evbuf, pkt.get_buffer(), total_len);
            pkt.InitialFromBuffer(total_len);
            MsgData& msg_data = pkt.get_msg_data();
            PacketDispatcher::GetInstance().DeliverPacket(conn, msg_data.get_avatar_id(),
                                                          msg_data.get_msg_name(), msg_data.get_msg(),
                                                          msg_data.get_msg_size());

            readable = evbuffer_get_length(evbuf);
            continue;
        } else {
            break;
        }
    }
}
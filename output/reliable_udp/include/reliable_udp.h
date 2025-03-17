#ifndef RELIABLE_UDP_H
#define RELIABLE_UDP_H

#if defined(WIN32)
#if defined(JD_DLL_EXPORT)
#define UDP_API_EXPORT __declspec(dllexport)
#else
#define UDP_API_EXPORT __declspec(dllimport)
#endif
#else  // defined(__linux__) || defined(__GNUC__)
#define UDP_API_EXPORT
#endif

#if defined(__GNUC__)
#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif
#endif

#include <functional>
#include <memory>
#include <string>

namespace RELIABLE_UDP_NS {

const static std::string UNICAST_PUB_CHANNEL = "aeron:udp?endpoint=192.168.*.*:20121";
const static std::string UNICAST_SUB_CHANNEL = "aeron:udp?endpoint=127.0.0.1:20121";
const static std::string MULTICAST_CHANNEL = "aeron:udp?endpoint=224.0.0.1:40456";
const static std::int32_t DEFAULT_STREAM_ID = 1001;

using callback_t = std::function<void(const char* msg, const size_t length)>;

struct Settings {
    std::string channel;
    std::int32_t streamId = DEFAULT_STREAM_ID;
    callback_t callback;
};

enum class PubReturnCode : int64_t {
    SUCCESS = 0,
    NOT_CONNECTED = -1,
    BACK_PRESSURED = -2,
    ADMIN_ACTION = -3,
    PUBLICATION_CLOSED = -4,
    MAX_POSITION_EXCEEDED = -5,
};

struct UDPMsgHead {
    uint32_t user_data;
    uint32_t msg_type;
    uint32_t msg_count;
    uint32_t msg_len;

    UDPMsgHead(uint32_t userData = 0, uint32_t msgType = 0, uint32_t msgCount = 1,
               uint32_t msgLen = 0)
        : user_data(userData), msg_type(msgType), msg_count(msgCount), msg_len(msgLen) {}
};

class UDP_API_EXPORT ReliableUDP {
   public:
    static std::shared_ptr<ReliableUDP> Create();

    virtual int InitPublisher() = 0;
    virtual int InitPublisher(Settings& setting) = 0;
    virtual bool Connected() = 0;
    virtual bool GetPubStatus() = 0;

    virtual PubReturnCode PubMsg(const std::string& msg) = 0;
    virtual PubReturnCode PubMsg(const char* msg, const size_t len) = 0;
    virtual PubReturnCode PubMsg(UDPMsgHead& head, const char* body) = 0;

    virtual int InitSubscriber() = 0;
    virtual int InitSubscriber(Settings& setting) = 0;
    virtual void StartSubThread() = 0;
    virtual bool GetSubStatus() = 0;
};
}  // namespace RELIABLE_UDP_NS

#if defined(__GNUC__)
#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif
#endif

#endif

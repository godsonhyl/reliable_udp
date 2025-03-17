#ifndef UDP_CAST_IMPL
#define UDP_CAST_IMPL

#include <aeron/Aeron.h>

#include "reliable_udp.h"

using namespace aeron;

namespace RELIABLE_UDP_NS {

class ReliableUDPImpl : public ReliableUDP {
   public:
    ReliableUDPImpl();
    ~ReliableUDPImpl();

    int InitPublisher() override;
    int InitPublisher(Settings& setting) override;
    bool Connected() override;
    bool GetPubStatus() override;

    PubReturnCode PubMsg(const std::string& msg) override;
    PubReturnCode PubMsg(const char* msg, const size_t len) override;
    PubReturnCode PubMsg(UDPMsgHead& head, const char* body) override;

    int InitSubscriber() override;
    int InitSubscriber(Settings& setting) override;
    void StartSubThread() override;
    bool GetSubStatus() override;

   private:
    PubReturnCode MultiCastMsg(concurrent::AtomicBuffer& buff, const size_t len);
    void SubscriberLoop();

   private:
    Settings pub_setting_;
    Settings sub_setting_;

    std::shared_ptr<Aeron> aeron_pub_;
    std::shared_ptr<Aeron> aeron_sub_;

    std::shared_ptr<Publication> pub_;
    std::shared_ptr<Subscription> sub_;

    std::atomic<bool> running_;
    std::shared_ptr<std::thread> sub_thread_;
};
}  // namespace RELIABLE_UDP_NS

#endif
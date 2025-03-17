#include "reliable_udp_impl.h"

#include <aeron/Aeron.h>
#include <aeron/FragmentAssembler.h>
#include <aeron/concurrent/AgentRunner.h>
#include <aeron/concurrent/SleepingIdleStrategy.h>

#include <cinttypes>
#include <csignal>
#include <iostream>
#include <memory>
#include <thread>

using namespace aeron;

namespace RELIABLE_UDP_NS {

std::shared_ptr<ReliableUDP> ReliableUDP::Create() { return std::make_shared<ReliableUDPImpl>(); }

ReliableUDPImpl::ReliableUDPImpl() { running_.store(true); }

ReliableUDPImpl::~ReliableUDPImpl() {
    running_ = false;
    if (sub_thread_) {
        sub_thread_->join();
    }
}

int ReliableUDPImpl::InitPublisher(Settings& setting) {
    pub_setting_ = setting;
    return this->InitPublisher();
}

int ReliableUDPImpl::InitPublisher() {
    Context context;
    context.newPublicationHandler([](const std::string& channel, std::int32_t streamId,
                                     std::int32_t sessionId, std::int64_t correlationId) {
        std::cout << "Publication: " << channel << " " << correlationId << ":" << streamId << ":"
                  << sessionId << std::endl;
    });

    aeron_pub_ = Aeron::connect(context);
    auto id = aeron_pub_->addPublication(pub_setting_.channel, pub_setting_.streamId);
    pub_ = aeron_pub_->findPublication(id);
    while (!pub_) {
        std::this_thread::yield();
        pub_ = aeron_pub_->findPublication(id);
    }

    return 0;
}

bool ReliableUDPImpl::Connected() { return (pub_ && pub_->isConnected()); }

bool ReliableUDPImpl::GetPubStatus() {
    return (pub_ && pub_->channelStatus() == ChannelEndpointStatus::CHANNEL_ENDPOINT_ACTIVE);
}

PubReturnCode ReliableUDPImpl::PubMsg(const std::string& msg) {
    return PubMsg(msg.c_str(), msg.size());
}

PubReturnCode ReliableUDPImpl::PubMsg(const char* msg, const size_t len) {
    std::vector<std::uint8_t> buffer(len);

    concurrent::AtomicBuffer atomic_buffer(buffer.data(), len);

    atomic_buffer.putBytes(0, reinterpret_cast<const std::uint8_t*>(msg), len);

    return MultiCastMsg(atomic_buffer, len);
}

PubReturnCode ReliableUDPImpl::PubMsg(UDPMsgHead& head, const char* body) {
    const static size_t head_len = sizeof(UDPMsgHead);
    size_t msg_length = head_len + head.msg_len;

    std::vector<std::uint8_t> buffer(msg_length);
    concurrent::AtomicBuffer atomic_buffer(buffer.data(), msg_length);

    atomic_buffer.putBytes(0, reinterpret_cast<const std::uint8_t*>(&head), head_len);
    atomic_buffer.putBytes(head_len, reinterpret_cast<const std::uint8_t*>(body), head.msg_len);

    return MultiCastMsg(atomic_buffer, msg_length);
}

PubReturnCode ReliableUDPImpl::MultiCastMsg(concurrent::AtomicBuffer& buff, const size_t len) {
    auto ret = pub_->offer(buff, 0, len);
    if (ret >= 0) {
        return PubReturnCode::SUCCESS;
    } else {
        return static_cast<PubReturnCode>(ret);
    }
}

int ReliableUDPImpl::InitSubscriber() {
    Context context;

    context
        .newSubscriptionHandler(
            [](const std::string& channel, std::int32_t streamId, std::int64_t correlationId) {
                std::cout << "Subscription: " << channel << " " << correlationId << ":" << streamId
                          << std::endl;
            })
        .availableImageHandler([](Image& image) {
            std::cout << "Available image correlationId=" << image.correlationId()
                      << " sessionId=" << image.sessionId() << " at position=" << image.position()
                      << " from " << image.sourceIdentity() << std::endl;
        })
        .unavailableImageHandler([](Image& image) {
            std::cout << "Unavailable image on correlationId=" << image.correlationId()
                      << " sessionId=" << image.sessionId() << " at position=" << image.position()
                      << " from " << image.sourceIdentity() << std::endl;
        });

    aeron_sub_ = Aeron::connect(context);
    // signal(SIGINT, sigIntHandler);

    auto id = aeron_sub_->addSubscription(sub_setting_.channel, sub_setting_.streamId);

    sub_ = aeron_sub_->findSubscription(id);
    while (!sub_) {
        std::this_thread::yield();
        sub_ = aeron_sub_->findSubscription(id);
    }

    return 0;
}

int ReliableUDPImpl::InitSubscriber(Settings& setting) {
    sub_setting_ = setting;
    return this->InitSubscriber();
}

bool ReliableUDPImpl::GetSubStatus() {
    return (sub_ && sub_->channelStatus() == ChannelEndpointStatus::CHANNEL_ENDPOINT_ACTIVE);
}

void ReliableUDPImpl::SubscriberLoop() {
    fragment_handler_t handle = [&](const AtomicBuffer& buffer, util::index_t offset,
                                    util::index_t length, const Header& header) {
        sub_setting_.callback(reinterpret_cast<const char*>(buffer.buffer()) + offset,
                              static_cast<size_t>(length));
    };

    static const int FRAGMENTS_LIMIT = 10;
    static const std::chrono::duration<long, std::milli> IDLE_SLEEP_MS(1);

    FragmentAssembler fragmentAssembler(handle);
    fragment_handler_t handler = fragmentAssembler.handler();
    SleepingIdleStrategy idleStrategy(IDLE_SLEEP_MS);

    try {
        while (running_) {
            const int fragmentsRead = sub_->poll(handler, FRAGMENTS_LIMIT);
            idleStrategy.idle(fragmentsRead);
        }
    } catch (const std::exception& e) {
        std::cerr << "subscriber thread error: " << e.what() << std::endl;
    }
}

void ReliableUDPImpl::StartSubThread() {
    sub_thread_ = std::make_shared<std::thread>(&ReliableUDPImpl::SubscriberLoop, this);
}

};  // namespace RELIABLE_UDP_NS

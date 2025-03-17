#include <iostream>
#include <thread>

#include "reliable_udp.h"

using namespace RELIABLE_UDP_NS;

callback_t callback = [](const char* msg, size_t len) {
    std::cout << "received a message: " << len << " " << msg << std::endl;
    return 0;
};

int main() {
    Settings setting;
    setting.channel = "aeron:udp?endpoint=224.0.0.1:48888";
    setting.callback = callback;

    auto udp_multi_ = ReliableUDP::Create();
    udp_multi_->InitSubscriber(setting);
    if (!udp_multi_->GetSubStatus()) {
        std::cout << "sub channel status is not active" << std::endl;
    } else {
        std::cout << "sub channel status is active" << std::endl;
    }

    udp_multi_->StartSubThread();

    getchar();

    return 0;
}
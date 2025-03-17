#include <iostream>
#include <thread>

#include "reliable_udp.h"

using namespace RELIABLE_UDP_NS;

callback_t callback = [](const char* msg, size_t len) {
    std::cout << "received a message: " << len << ", " << msg << std::endl;
    return 0;
};

void test_udp_cast() {
    Settings setting;
    setting.channel = "aeron:udp?endpoint=224.0.0.1:48888";
    setting.streamId = 1008;
    setting.callback = callback;

    auto udp_multi_ = ReliableUDP::Create();
    udp_multi_->InitSubscriber(setting);
    if (!udp_multi_->GetSubStatus()) {
        std::cout << "pub channel status is not active" << std::endl;
    } else {
        std::cout << "pub channel status is active" << std::endl;
    }

    udp_multi_->StartSubThread();

    udp_multi_->InitPublisher(setting);
    while (!udp_multi_->Connected()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (size_t i = 0; i < 10; i++) {
        std::string msg = "hello world, areon! hello world, areon! " + std::to_string(i);

        auto ret = udp_multi_->PubMsg(msg);
        if (ret == PubReturnCode::SUCCESS) {
            std::cout << "pub msg success" << std::endl;
        } else {
            switch (ret) {
                case PubReturnCode::NOT_CONNECTED: {
                    std::cout << "pub msg failed due to not connected to a subscriber" << std::endl;
                    break;
                }
                case PubReturnCode::BACK_PRESSURED: {
                    std::cout << "pub msg failed due to back pressure" << std::endl;
                    break;
                }
                case PubReturnCode::ADMIN_ACTION: {
                    std::cout << "pub msg failed due to an administration action" << std::endl;
                    break;
                }
                case PubReturnCode::PUBLICATION_CLOSED: {
                    std::cout << "pub msg failed because publication is closed" << std::endl;
                    break;
                }
                case PubReturnCode::MAX_POSITION_EXCEEDED: {
                    break;
                }
                default:
                    break;
            }
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
    test_udp_cast();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    test_udp_cast();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}
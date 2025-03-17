## reliable_udp

[Aeron](https://github.com/real-logic/aeron) 是一个高性能的消息传递库，专注于低延迟和高吞吐量的通信。尽管 UDP 本身是一个不可靠的传输协议（无连接、不保证消息顺序、不保证消息到达），但 Aeron 通过多种机制在 UDP 的基础上实现了可靠的消息传递。


reliable_udp 基于Aeron封装了一套简单易用的可靠udp接口，包括UDP unicast 和 UDP multicast

example目录提供了部分case：

* udp单播发送：pub_unicast_test

* udp单播接收：sub_unicast_test

* udp组播发送：pub_multicast_test

* udp组播接收：sub_multicast_test

* udp组播同时收发：pub_sub_test


是否可靠还需进一步验证

* 计划通过订阅转发RTS回报数据验证udp单播可靠性
* [mkt_forward](https://codeserver.jinde.local/huyl/mkt_forward)通过转发lightfeed行情验证udp组播可靠性 









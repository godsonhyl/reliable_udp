## reliable_udp

[Aeron](https://github.com/real-logic/aeron) 是一个高性能的消息传递库，专注于低延迟和高吞吐量的通信。尽管 UDP 本身是一个不可靠的传输协议（无连接、不保证消息顺序、不保证消息到达），但 Aeron 通过多种机制在 UDP 的基础上实现了可靠的消息传递。


reliable_udp 基于Aeron封装了一套简单易用的可靠udp接口，包括UDP unicast 和 UDP multicast

example目录提供了部分case：

* udp单播发送：pub_unicast_test

* udp单播接收：sub_unicast_test

* udp组播发送：pub_multicast_test

* udp组播接收：sub_multicast_test

* udp组播同时收发：pub_sub_test


reliable_udp是否真的可靠还需进一步大量测试验证

## 编译与运行

#### 编译环境说明

* c++17
* windows: vs2022或clang19.1
  ```
  ./setup_scripts/Setup_VS2022_64_RelWithDebInfo.bat
  ```

* linux: gcc9.2.0
  ```
  cd ${project_dir}
  mkdir build
  cd build && cmake .. && cmake -j8 && make install && cd -
  ```

#### 启动demo应用程序

在 Aeron 的 C++ 版本中，aeronmd 是 Aeron 的 Media Driver（媒体驱动），它是 Aeron 的核心组件之一，负责管理低级别的网络通信和内存映射文件。要运行 Aeron C++ 应用程序，首先需要启动 aeronmd，然后再运行demo程序。

* 启动aeronmd
  ```
  ${project_dir}/tools/linux/aeronmd
  ```

* 启动demo
  ```
  ${project_dir}/bin/pub_multicast_test
  ${project_dir}/bin/sub_multicast_test
  ...
  ```

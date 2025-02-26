# NetLogViewer
NetLogViewer Qt5

# NetLog Server files
main.cpp
NetLogViewer.h
NetLogViewer.cpp

# NetLog Client files
LogSender.h
LogSender.cpp

# Introduction 简介
当前简化版网络日志参考ios开发中的网络日志方案。
The current simplified version of the network log refers to the network log scheme in iOS development.

客户端LogSender通过QLocalSocket发送文本日志或程序快照日志给服务端NetLogViewer。
The client LogSender sends text logs or program snapshot logs to the server NetLogViewer through QLocalSocket.

服务端NetLogViewer显示文本日志和当前最新的程序快照。
The server-side NetLogViewer displays text logs and the latest program snapshot.

对使用客户端LogSender的程序影响较小，方便通过网络日志观察程序的行为。
The impact on programs using the client LogSender is minimal, making it convenient to observe program behavior through network logs.

# build
服务端：用vs或其它IDE创建一个qt5工程，用main.cpp 、NetLogViewer.h、NetLogViewer.cpp覆盖替换即可编译运行。
Server side: Create a qt5 project using VS or other IDEs, and replace it with main.cpp, NetLogViewer. h, or NetLogViewer. cpp to compile and run.

客户端：将LogSender.h、LogSender.cpp加入工程即可使用。
Client: Add LogSender. h and LogSender. cpp to the project to use.


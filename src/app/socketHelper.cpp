/**
 * @file socketHelper.cpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief socket的部分实现函数
 * @version 0.1
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifdef __linux__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "log.hpp"

static const int serverPort = 9000;
static const char *serverPath = "/tmp/test.sock";

int clientInitUnix(const char *unixPath) {
    /********** 1. 创建套接字 **********/
    auto fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG_ERROR("socket error, unixPath {}, error {}:{}", unixPath, errno, strerror(errno));
        return -1;
    }

    /********** 2. 连接服务器 **********/
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, unixPath);
    auto ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERROR("connect error, unixPath {}, error {}:{}", unixPath, errno, strerror(errno));
        return -1;
    }
    return fd;
}

int clientInitPort(const char *ip, int port) {
    /********** 1. 创建套接字 **********/
    auto fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        LOG_ERROR("socket error");
        return -1;
    }

    /********** 2. 连接服务器 **********/
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;             // ipv4
    addr.sin_port = htons(port);           // 端口号转换为网络字节序
    addr.sin_addr.s_addr = inet_addr(ip);  // 连接本机的服务器
    // 连接服务端
    auto ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERROR("connect error");
        return -1;
    }

    return fd;
}

#define SEND_FD_NUM 1  // 可以一次发送多个文件描述符
void sendFd(int clientFd) {
    /********** 1. 和待接受的进程建立连接 **********/
    // 必须使用unix套接字才能进行文件描述符发送
    auto fd = clientInitUnix(serverPath);
    if (fd < 0) {
        LOG_ERROR("clientInitUnix error");
        return;
    }

    /********** 2. 构建msg进行发送套接字 **********/
    struct msghdr msg = {0};
    // 使用union定义可以使用宏设置字节对齐的结构体
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(clientFd) *
                                SEND_FD_NUM)];  // 包含cmsghdr大小和fd大小的空间，对两个都进行了8字节对齐
    } control_un;
    control_un.cm.cmsg_len =
        CMSG_LEN(sizeof(clientFd) * SEND_FD_NUM);  // 此宏代表cmsghdr大小和clientFd大小的空间，对cmsghdr进行了8字节对齐
    control_un.cm.cmsg_level = SOL_SOCKET;          // 发送文件描述符必须设置的level
    control_un.cm.cmsg_type = SCM_RIGHTS;           // 发送文件描述符
    int *fdPtr = (int *)CMSG_DATA(&control_un.cm);  // 获取文件描述符的指针
    fdPtr[0] = clientFd;                            // 设置文件描述符
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    // 下面的设置是必须的，用于标识是否发送成功，data可以随意设置大小
    struct iovec vec = {0};
    unsigned char data = 0;
    vec.iov_base = &data;
    vec.iov_len = sizeof(data);
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    LOG_DEBUG("msg_controllen {}, control_un.cm.cmsg_len {}", msg.msg_controllen, control_un.cm.cmsg_len);

    /********** 3. 发送文件描述符 **********/
    auto ret = sendmsg(fd, &msg, 0);
    if (ret < 0) {
        LOG_ERROR("sendmsg error");
        return;
    }

    /********** 4. 关闭连接套接字，发送的文件描述符是否关闭外部处理 **********/
    close(fd);
}

int recvFd(int clientFd) {
    /********** 1. 构建msg进行接收文件描述符 **********/
    struct msghdr msg = {0};
    // 这里留下可接受的空间即可，其他不用设置
    union {
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(clientFd))];  // 只接受一个文件描述符，所以只需要一个空间
    } control_un;
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
    // 这里使用同样的或者更大的内存进行接受都可以，但是必须分配内存进行接受
    struct iovec vec = {0};
    unsigned char data = 0;
    vec.iov_base = &data;
    vec.iov_len = sizeof(data);
    msg.msg_iov = &vec;
    msg.msg_iovlen = 1;
    LOG_DEBUG("msg_controllen {}, control_un.cm.cmsg_len {}", msg.msg_controllen, control_un.cm.cmsg_len);

    /********** 3. 接受文件描述符 **********/
    auto ret = recvmsg(clientFd, &msg, 0);
    if (ret < 0) {
        LOG_ERROR("recvmsg error");
        return -1;
    }

    /********** 4. 判断接收是否合法 **********/
    cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);
    if (cmptr == nullptr                                  // 要存在地址
        || cmptr->cmsg_len != CMSG_LEN(sizeof(clientFd))  // 长度要够
        || cmptr->cmsg_level != SOL_SOCKET                // level要正确
        || cmptr->cmsg_type != SCM_RIGHTS) {              // type要正确
        LOG_ERROR("CMSG_FIRSTHDR error");
        return -1;
    }
    int *fdPtr = (int *)CMSG_DATA(cmptr);

    /********** 5. 返回fd **********/
    return fdPtr[0];
}

int serverInitPort(int port) {
    /********** 1. 创建套接字 **********/
    auto fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
        LOG_ERROR("socket error");
        return -1;
    }

    /********** 2. 绑定端口 **********/
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;                 // ipv4
    addr.sin_port = htons(port);               // 端口号转换为网络字节序
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 监听所有地址 0.0.0.0
    auto ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERROR("bind error");
        return -1;
    }

    /********** 3. 监听端口，这里可以通过netstat查看到 **********/
    ret = listen(fd, SOMAXCONN);  // 队列长度为SOMAXCONN，即最大排队连接数
    if (ret < 0) {
        LOG_ERROR("listen error");
        return -1;
    }

    return fd;
}

int serverInitUnix(const char *unixPath) {
    /********** 1. 创建套接字 **********/
    auto fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG_ERROR("socket error, unixPath {}, error {}:{}", unixPath, errno, strerror(errno));
        return -1;
    }

    /********** 2. 绑定域套接字，不存在会创建 **********/
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, unixPath, sizeof(addr.sun_path) - 1);
    auto ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERROR("bind error, unixPath {}, error {}:{}", unixPath, errno, strerror(errno));
        return -1;
    }

    /********** 3. 监听域套接字，这里可以通过netstat查看到 **********/
    ret = listen(fd, SOMAXCONN);  // 队列长度为SOMAXCONN，即最大排队连接数
    if (ret < 0) {
        LOG_ERROR("listen error, unixPath {}, error {}:{}", unixPath, errno, strerror(errno));
        return -1;
    }

    return fd;
}

void serverRun(int serverFd, std::function<void(int)> handler) {
    while (true) {
        /********** 1. 接受客户端连接 **********/
        sockaddr_in clientAddr = {0};
        socklen_t clientAddrLen = sizeof(clientAddr);
        // 下面函数会直接阻塞，直到有客户端连接进来。后两个参数用于获取客户端的地址信息
        auto clientFd = accept(serverFd, (sockaddr *)&clientAddr, &clientAddrLen);
        if (clientFd < 0) {
            LOG_ERROR("accept error");
            continue;
        }
        LOG_DEBUG("accept client {}, addr {}:{}", clientFd, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        /********** 2. 处理客户端请求 **********/
        handler(clientFd);

        /********** 3. 关闭客户端套接字 **********/
        close(clientFd);
    }
}

void clientRunPort() {
    /********** 1. 创建套接字 **********/
    auto fd = clientInitPort("127.0.0.1", serverPort);
    if (fd < 0) {
        LOG_ERROR("clientInitPort error");
        return;
    }

    /********** 2. 发送消息 **********/
    auto msg = "hello";
    // recv和send拿到的都是不包含tcp头部的数据
    auto ret = send(fd, msg, strlen(msg), 0);
    if (ret < 0) {
        LOG_ERROR("send error");
        return;
    }

    /********** 3. 接收服务端的消息 **********/
    char buf[1024] = {0};
    // recv函数会直接阻塞，直到服务端发送消息过来
    ret = recv(fd, buf, sizeof(buf), 0);
    if (ret < 0) {
        LOG_ERROR("recv error");
        return;
    }
    LOG_DEBUG("recv msg {}", buf);

    /********** 4. 关闭客户端套接字 **********/
    close(fd);
}

void sendMsgServerRun() {
    auto serverFd = serverInitPort(serverPort);
    if (serverFd < 0) {
        LOG_ERROR("serverInitPort error");
        return;
    }
    LOG_INFO("serverFd {}", serverFd);

    serverRun(serverFd, [](int clientFd) { sendFd(clientFd); });
}

void printClientAddr(int clientFd, const char *tag) {
    // 打印访问日志
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    getpeername(clientFd, (struct sockaddr *)&sa, &len);
    LOG_INFO("{}, fd {}, client addr {}:{}", tag, clientFd, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
}

void recvMsgServerRun() {
    auto serverFd = serverInitUnix(serverPath);
    if (serverFd < 0) {
        LOG_ERROR("serverInitUnix error");
        return;
    }
    LOG_INFO("serverFd {}", serverFd);

    serverRun(serverFd, [](int clientFd) {
        auto fdRecv = recvFd(clientFd);
        if (fdRecv < 0) {
            LOG_ERROR("recvFd error");
            return;
        }

        printClientAddr(fdRecv, "recv fd");

        // 接收消息
        char buf[1024] = {0};
        auto ret = recv(fdRecv, buf, sizeof(buf), 0);
        if (ret < 0) {
            LOG_ERROR("recv error");
            return;
        }
        LOG_INFO("fd {}, recv data {}", fdRecv, buf);

        // 发送消息
        send(fdRecv, buf, strlen(buf), 0);

        // 关闭fd
        close(fdRecv);
    });
}

#endif  // __linux__

/**
 * @file epollWorker.cpp
 * @brief epoll服务端和客户端程序
 * @author wangyubo
 * @version 0.1
 * @date 2021-01-06
 */

#include "epollWorker.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "log.hpp"

#define MAC_CONN_NUM 512
#define EPOLL_TIMEOUT 1000  // 1000毫秒超时

int g_iEpfd;

/*
 *将文件描述符设置为非阻塞
 * @param [in]
 *     fd : file descriptor
 * @return
 *     0 if success or -1 for error
 */
int SetNonblockFD(int fd) {
    int opts = 0;
    opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        return -1;
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0) {
        return -1;
    }
    return 0;
}

// Unix取值
#define UNIX_PATH_MAX 108

/*
 * 创建异步TCP监听套接字
 * @param [in]
 *     sHost : host ip
 *     port : host  port
 *     listenQ : listen queue length
 * @param [out]
 *    error : can be displayed by strerror(*error)
 * @return
 *     connection fd if success or -1 for error
 */
int CreateAsyncTcpListener(int listenQ) {
    // 创建socket
    int tcplistenFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (tcplistenFd < 0) {
        LOG_ERROR("create socket failed {}", strerror(errno));
        return -1;
    }

    // 设置linger
    struct linger sLin;
    sLin.l_onoff = 1;
    sLin.l_linger = 0;
    int ret =
        setsockopt(tcplistenFd, SOL_SOCKET, SO_LINGER, &sLin, sizeof(sLin));
    if (ret < 0) {
        LOG_ERROR("setsockopt so_linger failed {}", strerror(errno));
        close(tcplistenFd);
        return -1;
    }
    // 设置reuseaddr
    int lFlag = 1;
    ret =
        setsockopt(tcplistenFd, SOL_SOCKET, SO_REUSEADDR, &lFlag, sizeof(int));
    if (ret < 0) {
        LOG_ERROR("setsockopt so_reuseaddr failed {}", strerror(errno));
        close(tcplistenFd);
        return -1;
    }
    // 绑定域套接字
    sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, "test.sock");
    (void)unlink("test.sock");
    if (bind(tcplistenFd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        LOG_ERROR("bind socket failed {}", strerror(errno));
        close(tcplistenFd);
        return -1;
    }
    // 设置非阻塞socket
    if (SetNonblockFD(tcplistenFd) < 0) {
        LOG_ERROR("set no block failed {}", strerror(errno));
        close(tcplistenFd);
        return -1;
    }
    // 监听socket
    if (listenQ < 0) listenQ = 0;
    if (listen(tcplistenFd, listenQ) < 0) {
        LOG_ERROR("listen failed {}", strerror(errno));
        close(tcplistenFd);
        return -1;
    }
    return tcplistenFd;
}

/*
 * 设置tcp fd
 * keepalive包的时间间隔，次数是否开启keepalive,以及无数据探测等待时间
 * @param [in]
 *     fd : tcp连接的文件描述符
 *     isKeepAlive :
 * 是否开启keepalive，如果不开启，会按系统默认时间发keepalive包 idleTime :
 * 等待idleTime秒后才进行发keepalive包 sendInterval : 发送keepalive包间隔秒数
 *    sendCount : 发送次数
 * @return
 *     0 if success or -1 for error
 */
int SetKeepAliveFD(int fd, int isKeepAlive, int idleTime, int sendInterval,
                   int sendCount) {
    int keepAlive = isKeepAlive;  // 开启keepalive属性
    int keepIdle =
        idleTime;  // 如该连接在idleTime秒内没有任何数据往来,则进行探测
    int keepInterval = sendInterval;  // 探测时发包的时间间隔为5 秒
    int keepCount =
        sendCount;  // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.

    int ret = 0;
    ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive,
                     sizeof(keepAlive));
    if (ret < 0) {
        return -1;
    }
    ret = setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle,
                     sizeof(keepIdle));
    if (ret < 0) {
        return -1;
    }
    ret = setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval,
                     sizeof(keepInterval));
    if (ret < 0) {
        return -1;
    }
    ret = setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount,
                     sizeof(keepCount));
    if (ret < 0) {
        return -1;
    }
    return 0;
}

int _DoRecvConnect(int listenFd) {
    int connfd = accept(listenFd, NULL, NULL);
    while (connfd < 0) {
        if (errno == EINTR) {
            connfd = accept(listenFd, NULL, NULL);
        } else {
            LOG_WARN("_DoRecvConnect accept error {}", strerror(errno));
            return -1;
        }
    }
    // //设置keep alive
    // if (SetKeepAliveFD(connfd, 1, 60, 1, 1) < 0) {
    //     close(connfd);
    //     LOG_WARN("Set keep alive failure error {}", strerror(errno));
    //     return -1;
    // }

    if (SetNonblockFD(connfd) < 0) {
        LOG_WARN("SetNonblockFD error {}", strerror(errno));
        close(connfd);
        connfd = -1;
        return -1;
    }

    return connfd;
}

int AddSockToEv(int socket) {
    struct epoll_event ev;
    ev.data.u32 = socket;
    ev.events = EPOLLIN;
    if (0 != epoll_ctl(g_iEpfd, EPOLL_CTL_ADD, socket, &ev)) {
        LOG_ERROR("epoll_ctl add error fd {} {}", socket, strerror(errno));
        return -1;
    }
    return 0;
}

void _DoHandleListenerMsg(struct epoll_event *ev) {
    int socket = -1;

    assert(ev != NULL);
    if (ev->events & EPOLLIN) {
        socket = _DoRecvConnect(ev->data.fd);
        AddSockToEv(socket);
    } else if ((ev->events & EPOLLERR) || (ev->events & EPOLLHUP))  //该fd错误
    {
        LOG_ERROR("_DoRecvConnect epoll  EPOLLERR or EPOLLHUP, pipd will exit");
        return;  //错误发生，退出
    } else {
        LOG_ERROR("listenfd {} unknown epoll events {}", (int)(ev->data.fd),
                  (uint32_t)(ev->events));
        return;
    }
}

#define SEND_MAX_RETRY 1
int SocketSend(int socket, unsigned char *pBuffer, int nBufferLen) {
    int nSendBytes = 0;

    for (int i = 0; i < SEND_MAX_RETRY; i++) {
        nSendBytes = (int)send(socket, pBuffer, nBufferLen, 0);

        if (nSendBytes < 0 && (errno == EWOULDBLOCK || errno == EINTR)) {
            // usleep(0);
        } else {
            break;
        }
    }
    return 0;
}

#define MAX_RECV_BUF_LEN 1024
void _DoHandleMsg(struct epoll_event *ev) {
    int nHaveRcvLen = 0;
    unsigned char recvBuf[MAX_RECV_BUF_LEN] = {0};
    int iRet = 0;
    int socket = ev->data.fd;

    if (ev->events & EPOLLIN) {
        LOG_DEBUG("Recv from epoll");
        // ev->events = EPOLLOUT;
        nHaveRcvLen = (int)recv(socket, recvBuf, MAX_RECV_BUF_LEN, 0);
        LOG_HEX(recvBuf, nHaveRcvLen);
        SocketSend(socket, recvBuf, nHaveRcvLen);

        iRet = epoll_ctl(g_iEpfd, EPOLL_CTL_DEL, socket, ev);
        if (iRet < 0) {
            LOG_ERROR("_DoHandleMsg epoll del socket {} failed {}", socket,
                      strerror(errno));
        }
        close(socket);
        socket = -1;
    }
}

void serverRun() {
    int iMaxClient = 1000;

    //初始化epoll
    g_iEpfd = epoll_create(iMaxClient * 2 +
                           10);  //每个Worker处理最大用户数 *  2  + 10
    if (-1 == g_iEpfd) {
        LOG_ERROR("svcauthd epoll_create socket error:{}", strerror(errno));
        return;
    }

    int listenFd = CreateAsyncTcpListener(MAC_CONN_NUM);
    if (listenFd < 0) {
        LOG_ERROR("svcauthd CreateAsyncTcpListener failed");
        return;
    }

    struct epoll_event ev, events[1000 * 2 + 10];
    ev.events = EPOLLIN;
    ev.data.fd = listenFd;
    int ret = epoll_ctl(g_iEpfd, EPOLL_CTL_ADD, listenFd,
                        &ev);  //将监听套接字加入epoll
    if (ret < 0) {
        LOG_ERROR("[CPipdManager]_AddListener() epoll add failed error {}",
                  errno);
        return;
    }

    while (true) {
        ret = epoll_wait(g_iEpfd, events, 1000 * 2 + 10, EPOLL_TIMEOUT);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            LOG_ERROR("epoll_wait failed with error {}", strerror(errno));
            break;
        }
        if (ret != 0) {
            for (int i = 0; i < ret; ++i) {
                if (events[i].data.fd == listenFd) {
                    // 新连接建立
                    _DoHandleListenerMsg(&events[i]);
                } else {
                    _DoHandleMsg(&events[i]);
                }
            }
        }
    }

    return;
}

#include <iostream>
using namespace std;

void clientRun() {
    // 创建socket
    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketFd < 0) {
        LOG_ERROR("create socket failed {}", strerror(errno));
        return;
    }

    // 绑定域套接字
    sockaddr_un serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, "test.sock");
    if (connect(socketFd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        LOG_ERROR("connect socket failed {}", strerror(errno));
        close(socketFd);
        return;
    }

    char buf[255] = {0};
    string str;
    while (true) {
        cin >> str;
        send(socketFd, str.c_str(), str.length(), 0);
        memset(buf, 0, 255);
        int len = recv(socketFd, buf, 255, 0);
        LOG_HEX(buf, len);
    }
    close(socketFd);

    return;
}



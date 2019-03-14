/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define	UDP_TEST_PORT		80

int main(int argC, char* arg[])
{
    struct sockaddr_in addr;
    int sockfd, len = 0;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[256];

    /* 建立socket，注意必须是SOCK_DGRAM */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror ("socket");
        exit(1);
    }

    /* 填写sockaddr_in 结构 */
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_TEST_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY) ;// 接收任意IP发来的数据

    /* 绑定socket */
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0) {
        perror("connect");
        exit(1);
    }

    while(1) {
        bzero(buffer, sizeof(buffer));
        len = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                       (struct sockaddr *)&addr ,&addr_len);
        /* 显示client端的网络地址和收到的字符串消息 */
        printf("Received from client %s:%d  : %s\n",
               inet_ntoa(addr.sin_addr), addr.sin_port, buffer);
    }

    return 0;
}

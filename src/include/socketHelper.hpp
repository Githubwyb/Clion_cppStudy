/**
 * @file socketHelper.hpp
 * @author 王钰博18433 (18433@sangfor.com)
 * @brief socket的部分实现函数
 * @version 0.1
 * @date 2022-08-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __CLION_CPPSTUDY_SOCKETHELPER_HPP__
#define __CLION_CPPSTUDY_SOCKETHELPER_HPP__

#include <functional>

#ifdef __linux__

int clientInitUnix(const char *unixPath);
int clientInitPort(const char *ip, int port);

void sendFd(int clientFd);

int recvFd(int clientFd);

int serverInitPort(int port);

int serverInitUnix(const char *unixPath);

void serverRun(int serverFd, std::function<void(int)> handler);

void clientRunPort();

void sendMsgServerRun();

void printClientAddr(int clientFd, const char *tag);

void recvMsgServerRun();

#endif  // __linux__

#endif  // __CLION_CPPSTUDY_SOCKETHELPER_HPP__

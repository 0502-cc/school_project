#include "ytplc.h"
#include <winsock.h>
#include <QDebug>

#include <iostream>
#pragma comment(lib,"ws2_32.lib")

YtPlc* g_plc = nullptr;

YtPlc::YtPlc()
{
    socket_ = 0;
}

YtPlc::~YtPlc()
{

}

void YtPlc::Startup()
{
    WSADATA  Ws;
    WSAStartup(MAKEWORD(2, 2), &Ws);
    if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0) 
    {
        std::cout << "Failed to initialize Winsock" << endl;
    }
}

void YtPlc::Cleanup()
{
    WSACleanup();
}

bool YtPlc::Open(const char* addr, const int port, const int timeout_ms)
{
    if (socket_ > 0)
    {
        return true;
    }

    if (INVALID_SOCKET == (socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))//IPPROTO_UDP导致读取后无法操作
    {
        WSACleanup();
        return false;
    }
    //if (INVALID_SOCKET == (socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))//IPPROTO_UDP导致读取后无法操作
    //{
    //    return false;
    //}

    // set block timeout
    //setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(int));
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(int));

    linger linger_set;
    linger_set.l_onoff = 0, linger_set.l_linger = 0;
    setsockopt(socket_, SOL_SOCKET, SO_LINGER, (const char*)&linger_set, sizeof(linger));

    sockaddr_in socker_addr;
    memset(&socker_addr, 0, sizeof(socker_addr));

    socker_addr.sin_family = AF_INET;
    socker_addr.sin_addr.s_addr = inet_addr(addr);
    socker_addr.sin_port = htons(port);

    if (SOCKET_ERROR == connect(socket_, (sockaddr*)&socker_addr, sizeof(socker_addr)))
    {
        socket_ = (int)INVALID_SOCKET;
        return false;
    }
    InitParam();
    int OK[1] = { 1 };
    int w = PLCWrite(0xA8, &OK[0], 1, 4030);//扫码软件开启状态
    if (w == 0)
    {
        socket_ = 0;
        return false;
    }
    return true;
}

//bool YtPlc::Open(const char *addr, const int port, const int timeout_ms)
//{
//    // 如果套接字已打开，返回 true
//    if (socket_ > 0)
//    {
//        return true;
//    }
//
//    //if (INVALID_SOCKET == (socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))//IPPROTO_UDP导致读取后无法操作
//    //{
//    //    WSACleanup();
//    //    return false;
//    //}
//     // 尝试创建一个 TCP 套接字
//    if (INVALID_SOCKET == (socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))//IPPROTO_TCP导致读取后无法操作
//    {
//        std::cout << "TCP error" << endl;
//        WSACleanup();
//        return false;
//    }
//
//    // 设置套接字的阻塞超时
//    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(int));
//
//
//
//    // 设置 SO_LINGER 选项，以确保在关闭套接字时立即关闭连接
//    linger linger_set;
//    linger_set.l_onoff = 0, linger_set.l_linger = 0;
//    setsockopt(socket_, SOL_SOCKET, SO_LINGER, (const char*)&linger_set, sizeof(linger));
//
//    // 配置服务器地址结构体
//    sockaddr_in socker_addr;
//    memset(&socker_addr, 0, sizeof(socker_addr));
//
//    socker_addr.sin_family = AF_INET;
//    socker_addr.sin_addr.s_addr = inet_addr(addr);
//    socker_addr.sin_port = htons(port);
//
//    if (SOCKET_ERROR == connect(socket_, (sockaddr*)&socker_addr, sizeof(socker_addr)))
//    {
//        int error = WSAGetLastError();
//        std::cout << "套接字连接失败，错误代码: " << error << std::endl;
//
//        socket_ = (int)INVALID_SOCKET;
//        WSACleanup();//新添加
//        return false;
//    }
//    InitParam();
//    int OK[1] = { 1 };
//    int w = PLCWrite(0xA8, &OK[0], 1, 4030);//扫码软件开启状态
//    if (w == 0)
//    {
//        closesocket(socket_);
//        WSACleanup();
//        return false;
//    }
//    return true;
//}

//bool YtPlc::Open(const char* addr, const int port, const int timeout_ms)
//{
//
//    socket_ = 0;
//     如果套接字已打开，返回 true
//    if (socket_ > 0)
//    {
//        return true;
//    }
//
//     尝试创建一个 TCP 套接字
//    if (INVALID_SOCKET == (socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
//    {
//        std::cout << "TCP error" << std::endl;
//        WSACleanup();
//        return false;
//    }
//
//     设置套接字的阻塞超时
//    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_ms, sizeof(int));
//
//     设置非阻塞方式
//    u_long non_blocking = 1;
//    if (ioctlsocket(socket_, FIONBIO, &non_blocking) != 0)
//    {
//        std::cout << "Failed to set non-blocking mode" << std::endl;
//        closesocket(socket_);
//        WSACleanup();
//        return false;
//    }
//
//     配置服务器地址结构体
//    sockaddr_in socker_addr;
//    memset(&socker_addr, 0, sizeof(socker_addr));
//
//    socker_addr.sin_family = AF_INET;
//    socker_addr.sin_addr.s_addr = inet_addr(addr);
//    socker_addr.sin_port = htons(port);
//
//     非阻塞连接
//    if (SOCKET_ERROR == connect(socket_, (sockaddr*)&socker_addr, sizeof(socker_addr)))
//    {
//        int error = WSAGetLastError();
//        if (error != WSAEWOULDBLOCK)
//        {
//            std::cout << "正常错误，不用担心：套接字连接失败，错误代码: " << error << std::endl;
//            std::cout << "非阻塞连接" << endl;
//            closesocket(socket_);
//            WSACleanup();
//            return false;
//        }
//
//         连接正在进行中，等待一段时间
//        fd_set write_set;
//        FD_ZERO(&write_set);
//        FD_SET(socket_, &write_set);
//
//        timeval timeout;
//        timeout.tv_sec = timeout_ms / 1000 ;  // 增加 5 秒等待时间
//        timeout.tv_usec = (timeout_ms % 1000) * 1000;
//
//        int select_result = select(0, nullptr, &write_set, nullptr, &timeout);
//        if (select_result <= 0 || !FD_ISSET(socket_, &write_set))
//        {
//            std::cout << "套接字连接超时或错误，错误代码: " << error << std::endl;
//            std::cout << "阻塞连接" << endl;
//            closesocket(socket_);
//            WSACleanup();
//            return false;
//        }
//    }
//
//     非阻塞连接成功，设置回阻塞模式
//    non_blocking = 0;
//    if (ioctlsocket(socket_, FIONBIO, &non_blocking) != 0)
//    {
//        std::cout << "Failed to set blocking mode" << std::endl;
//        closesocket(socket_);
//        WSACleanup();
//        return false;
//    }
//
//     其他初始化工作
//    InitParam();
//
//    int OK[1] = { 1 };
//    int w = PLCWrite(0xA8, &OK[0], 1, 4030);//扫码软件开启状态
//    if (w == 0)
//    {
//        std::cout << "PLC写入失败" << std::endl;
//        closesocket(socket_);
//        WSACleanup();
//        return false;
//    }
//
//    return true;
//}


void YtPlc::Close()
{
    if (INVALID_SOCKET != socket_)
        closesocket(socket_);

    socket_ = (int)INVALID_SOCKET;
}

bool YtPlc::IsOpen()
{
    return (socket_ != INVALID_SOCKET);
}

bool YtPlc::Transfer(const char *send_data, const int send_size, char *recv_data, int &recv_size)
{
    // <out> parameter clear
    recv_size = 0;

    if (INVALID_SOCKET == socket_)
        return false;

    const int kBlockSize = 512;
    char data[kBlockSize]{ 0 };

    // <in> parameter check
    if (NULL == send_data || 0 == send_size)
        return false;

    // if iSendSize error, return
    if (send_size != send(socket_, send_data, send_size, 0))
        return false;

    int iCurRecvSize = 0;
    while (iCurRecvSize >= 0)
    {
        // receive block size each time
        iCurRecvSize = recv(socket_, data, kBlockSize, 0);

        if (iCurRecvSize <= 0)
            break;

        for (int i = 0; i != iCurRecvSize; ++i)
        {
            recv_data[recv_size + i] = data[i];
        }

        recv_size += iCurRecvSize;

        if (iCurRecvSize != kBlockSize)
            break;
    }
    return (recv_size > 0);
}

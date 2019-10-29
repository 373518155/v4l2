#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include "slog.h"

#define BUF_SIZE (1024 * 1024)

#define SOCKS5_STATUS_INIT 0        // 建立连接前
#define SOCKS5_STATUS_CONNECTED 1  // 成功建立连接

const char *gsLog = "trident";

void handleConnection(int connfd);

int main(int argc, char** argv)
{
    int    listenfd, connfd;
    struct sockaddr_in     servaddr;
    int     n;

    slog(gsLog, "main start...");

    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        slog(gsLog, "Error!create socket error: %s(errno: %d)",strerror(errno),errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(6677);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        slog(gsLog, "Error!bind socket error: %s(errno: %d)",strerror(errno),errno);
        exit(0);
    }

    if( listen(listenfd, 10) == -1){
        slog(gsLog, "Error!listen socket error: %s(errno: %d)",strerror(errno),errno);
        exit(0);
    }

    slog(gsLog, "======waiting for client's request======");
    while(1){
        if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            slog(gsLog, "Error!accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        slog(gsLog, "accept a client");
        handleConnection(connfd);
    }

    close(listenfd);
}

void handleConnection(int connfd) {
    unsigned char recv_buff[BUF_SIZE];
    unsigned char send_buff[BUF_SIZE];

    int n;
    int status = SOCKS5_STATUS_INIT;

    while (1) {
        n = recv(connfd, recv_buff, BUF_SIZE, 0);
        slog(gsLog, "recv data");
        hlog(gsLog, recv_buff, n);
        if (status == SOCKS5_STATUS_INIT) {
            if (recv_buff[0] == 0x05 && recv_buff[1] == 0x01 && recv_buff[2] == 0x00) { // 无需认证
                status = SOCKS5_STATUS_CONNECTED;
                slog(gsLog, "建立socks5连接");
                send_buff[0] = 0x05;
                send_buff[1] = 0x00; // 无需认证
                send(connfd, send_buff, 2, 0);
            }
        } else if (status == SOCKS5_STATUS_CONNECTED) {
            slog(gsLog, "处理命令");
        }
    }

    close(connfd);
}


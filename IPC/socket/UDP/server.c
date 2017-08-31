#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>


#define BUFSIZE 512 * 1024

/* 采用抽象命名 需要使用 netstat -an 去查看 */
static const char *srv_domain = "@srv_domain";

int main(int argc, char** argv)
{
    char rx_buf[BUFSIZE];
    int sockfd, ret;
    socklen_t len;
    struct sockaddr_un srv_addr, clt_addr;
    /* 创建服务端sockfd */
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sockfd == -1)
        perror("cannot create srv fd.");

    unlink(srv_domain);
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sun_family = AF_UNIX;
    /* 在linux可以采用这种抽象命名方式，MAC OS下不行 */
    strncpy(srv_addr.sun_path, srv_domain, sizeof(srv_addr.sun_path) - 1);
    srv_addr.sun_path[0] = '\0';
    size_t srv_addr_len = offsetof(struct sockaddr_un, sun_path) + strlen(srv_domain);

    /* 绑定服务端sockfd到服务端地址 */
    ret = bind(sockfd, (struct sockaddr*)&srv_addr, srv_addr_len);
    if(ret == -1)
        perror("can not bind srv_addr");

    /* 获取RCVBUF大小 */
    int recvBufSize;
    len = sizeof(recvBufSize);
    ret = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufSize, &len);
    if(ret ==-1)
        perror("getsocket error");
    printf("Before setsockopt, SO_RCVBUF-%d\n",recvBufSize);
    /* 改变REVBUF大小 linux - /proc/slabinfo */
    recvBufSize = 8192 * 1024;
    while(-1 == setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufSize, len)) {
        perror("setsockopt error");
        recvBufSize >>= 1;
    }
    ret = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufSize, &len);
    if(ret ==-1)
        perror("getsocket error");
    printf("Set recv buf successful, SO_RCVBUF-%d\n",recvBufSize);

    int recvSize;
    memset(&clt_addr, 0, sizeof(clt_addr));
    len = sizeof(clt_addr);
    printf("==============wait for msg from clt====================\n");
    for(;;)
    {
        memset(rx_buf, 0, sizeof(rx_buf));
        recvSize = recvfrom(sockfd, rx_buf, sizeof(rx_buf), 0, (struct sockaddr*)&clt_addr, &len);
        if(recvSize == -1)
            perror("recvfrom error");
        printf("Recved message from clt(%s): %s\n", &clt_addr.sun_path[2], rx_buf);
    }
    close(sockfd);
}


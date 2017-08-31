#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>

/* 每次发送的 buffer 大小 */
#define BUFSIZE 20 * 1024

/* 本地socket服务端路径 */
static const char *srv_domain = "@srv_domain";
static const char *clt_domain = "@clt_domain";

int main(int argc, char** argv)
{
    char tx_buf[BUFSIZE];
    int sockfd, ret;
    socklen_t len;
    struct sockaddr_un srv_addr, clt_addr;
    //create pmmanager socket fd
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sockfd == -1)
        perror("cannot create clt fd");

    unlink(clt_domain);
    /* 配置客户端地址 抽象命名方式 */
    memset(&clt_addr, 0, sizeof(clt_addr));
    clt_addr.sun_family = AF_UNIX;
    snprintf(clt_addr.sun_path, sizeof(clt_addr.sun_path) - 1, "%s-%d", clt_domain, getpid());
    clt_addr.sun_path[0] = '\0';
    size_t clt_addr_len = offsetof(struct sockaddr_un, sun_path) + strlen(srv_domain);

    /* 绑定客户端地址 */
    ret = bind(sockfd, (struct sockaddr*)&clt_addr, clt_addr_len);
    if(ret == -1)
        perror("bind error");

    /* 获取SNDBUF大小，实际上UDP没有SNDBUF,只是写到该套接字的数据报大小限制 */
    int sendBufSize;
    len = sizeof(sendBufSize);
    ret = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, &len);
    if(ret ==-1)
        perror("getsocket error");
    printf("Before setsockopt, SO_SNDBUF-%d\n",sendBufSize);
    /* 突破默认数据包大小限制 */
    sendBufSize = 512*1024;
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, len);
    if(ret == -1)
        perror("setsockopt error");
    ret = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, &len);
    if(ret ==-1)
        perror("getsocket error");
    printf("Set send buf successful, SO_SNDBUF-%d\n\n\n", sendBufSize);

    /* 配置服务端地址 */
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sun_family = AF_UNIX;
    strncpy(srv_addr.sun_path, srv_domain, sizeof(srv_addr.sun_path) - 1);
    srv_addr.sun_path[0] = '\0';
    size_t srv_addr_len = offsetof(struct sockaddr_un, sun_path) + strlen(srv_domain);

    int sendSize = 0;
    int i = 1;
    do{
        memset(tx_buf, '0', sizeof(tx_buf));
        sprintf(tx_buf, "%d, send msg %d to pmmanager size %d", getpid(), i, BUFSIZE);
        printf("%s, msg size - %zu\n",tx_buf, sizeof(tx_buf));
        sendSize = sendto(sockfd, tx_buf, sizeof(tx_buf), 0, (struct sockaddr*)&srv_addr, srv_addr_len);
        /* 这里存在对端不存在，或者对端的RCVBUF满，则会将数据丢弃，需要考虑这两种情况。 */
        if(sendSize == -1)
            perror("sendto error");
        printf("Send message to pmmanager: %s\n\n\n", tx_buf);
    } while(i++);

    close(sockfd);
}

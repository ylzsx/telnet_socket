#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>
#include <ctype.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "wrap.h"

struct s_info {
    struct sockaddr_in clieaddr;
    int cfd;
};

void wait_child(int signo) {
    while (waitpid(0, NULL, WNOHANG) > 0);
    return;
}

void *do_work(void *arg) {
    int n;
    struct s_info *ts = (struct s_info *) arg;
    char buf[BUFSIZ];
    char str[INET_ADDRSTRLEN];  // #define INET_ADDRSTRLEN 16
    FILE *fp = NULL;

    while (1) {
        n = Read(ts->cfd, buf, sizeof(buf));
        buf[n] = '\0';
        if (n == 0) {
            printf("the client %s port %d closed at %s\n",
                    inet_ntop(AF_INET, &(*ts).clieaddr.sin_addr, str, sizeof(str)),
                    ntohs((*ts).clieaddr.sin_port), get_time());
            break;
        }
        printf("receive from %s at port %d at %s",
               inet_ntop(AF_INET, &(*ts).clieaddr.sin_addr, str, sizeof(str)),
               ntohs((*ts).clieaddr.sin_port), get_time());
        printf("operation: %s", buf);

        fp = popen(buf, "r");
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            Write(ts->cfd, buf, strlen(buf));
        }
        Write(ts->cfd, ">>> ", sizeof(">>> "));
    }
    fclose(fp);
    Close(ts->cfd);

    return (void *) 0;
}

int main() {

    pthread_t tid;
    int lfd, cfd;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    char clie_IP[BUFSIZ];
    struct s_info ts[256];  // 根据最大线程数创建结构体数组
    int i = 0;

    lfd = Socket(AF_INET, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); // 实现端口复用

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(lfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    Listen(lfd, 128);
    printf("Accepting client connect ...\n");

    signal(SIGCHLD, wait_child);    // 注册信号量回收子进程
    while (1) {
        clie_addr_len = sizeof(clie_addr);
        cfd = Accept(lfd, (struct sockaddr *) &clie_addr, &clie_addr_len);
        printf("client IP:%s, client port:%d connet\n",
               inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP)),
               ntohs(clie_addr.sin_port));

        ts[i].clieaddr = clie_addr;
        ts[i].cfd = cfd;

        // 达到线程最大数时， pthread_create出错处理，增加服务器稳定性
        pthread_create(&tid, NULL, do_work, (void *) &ts[i]);
        pthread_detach(tid);    // 子线程分离，防止僵尸线程产生
        i++;
    }

    return 0;
}
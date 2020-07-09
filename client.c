#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>
#include <time.h>

#include "wrap.h"

#define LOGOUT "logout\n\0"

int main(int argc, char *argv[]) {

    if (argc != 2) perr_exit("ip address is missing");
    char* serv_ip = argv[1];

    pid_t pid;
    int cfd;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZ], order[BUFSIZ];
    char str[INET_ADDRSTRLEN];
    int n;

    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));   // 结构体置空
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr.s_addr);

    Connect(cfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    printf("Welcome to connect service %s at %s",
            inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, str, sizeof(str)),
            get_time());

    pid = Fork();
    if (pid > 0) { // 父进程 发送请求给服务器
        printf(">>> ");
        while (fgets(order, sizeof(order), stdin)) {
            if (strncmp(order, LOGOUT, sizeof(LOGOUT)) == 0) break;
            if (Write(cfd, order, strlen(order)) == -1) {
                perr_exit("write error");
            }
        }
        printf("disconnection\n");
    } else if (pid == 0) { // 子进程 负责向标准输出写服务器返回的数据
        prctl(PR_SET_PDEATHSIG, SIGKILL);   // 设置信号量，使父进程退出时，子进程也退出
        while ((n = Read(cfd, buf, sizeof(buf))) > 0) {
            buf[n] = '\0';
            Write(STDOUT_FILENO, buf, n);
        }
        if (n == -1) {
            perr_exit("read error");
        }
    }

    Close(cfd);
    return 0;
}

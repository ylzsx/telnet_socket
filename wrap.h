#ifndef __WRAP_H_
#define __WRAP_H_

void perr_exit(const char *s);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Close(int fd);
pid_t Fork(void);

ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);

char *get_time();

#define SERV_PORT 5555

#endif //__WRAP_H_

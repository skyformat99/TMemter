#ifndef _TMEMTER_H
#define _TMEMTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>

class TMemter {
public:
	static const char* request;
	TMemter(int maxevents, int timeout, std::string ip, int port);
	virtual ~TMemter();
	void start();
	void conn();
protected:
	int set_nonblocking(int fd);
	void add_fd(int epoll_fd, int fd);
	bool writen(int sockfd, const char *buffer, int len);
	bool read_once(int sockfd, char *buf, int len);
	void close_conn(int epoll_fd, int sockfd);
private:
	int maxevents_;
	int timeout_;
	std::string ip_;
	int port_;
	int epollfd_;
};

#endif /* _TMEMTER_H */

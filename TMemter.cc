#include "TMemter.h"

const char* TMemter::request = "GET http://localhost/index.html HTTP/1.1\r\nConnection:keep-alive\r\n\r\nxxxxxxxxxx";

TMemter::TMemter(int maxevents, int timeout, std::string ip, int port) : maxevents_(maxevents), 
			timeout_(timeout), ip_(ip), port_(port), epollfd_(-1) { }

TMemter::~TMemter() { }

void TMemter::start() {
	// 创建epoll
	epollfd_ = epoll_create(maxevents_);
	// 发起连接
	conn();
	std::vector<epoll_event> events;
	events.reserve(maxevents_);
	char buffer[2048];

	while (true) {
		int fds = epoll_wait(epollfd_, &(*events.begin()), maxevents_, timeout_);
		for (int i=0; i<fds; ++i) {
			int sockfd = events[i].data.fd;
			// 读事件发生
			if (events[i].events & EPOLLIN) {
				if (! read_once(sockfd, buffer, 2048)) {
					close_conn(epollfd_, sockfd);
				}
				struct epoll_event event;
				event.events = EPOLLOUT | EPOLLET | EPOLLERR;
				event.data.fd = sockfd;
				epoll_ctl(epollfd_, EPOLL_CTL_MOD, sockfd, &event);
			} else if(events[i].events & EPOLLOUT) { // 写事件发生
				if (! writen(sockfd, request, strlen(request))) {
					close_conn(epollfd_, sockfd);
				}
				struct epoll_event event;
				event.events = EPOLLIN | EPOLLET | EPOLLERR;
				event.data.fd= sockfd;
				epoll_ctl(epollfd_, EPOLL_CTL_MOD, sockfd, &event);

			} else if (events[i].events & EPOLLERR) { // 异常发生
				close_conn(epollfd_, sockfd);
			}
			
		}
	}
}

void TMemter::conn() {
	int ret = 0;
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip_.c_str(), &address.sin_addr);
	address.sin_port = htons(port_);

	for (int i=0; i<maxevents_; ++i) {
		int sockfd = socket(PF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			continue;
		}

		if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) == 0) {
			printf("build connection %d\n", i);
			if (epollfd_ != -1) {
				add_fd(epollfd_, sockfd);
			}
		}
	}
}

int TMemter::set_nonblocking(int fd) {
	int oldOption = fcntl(fd, F_GETFL);
	int newOption = oldOption | O_NONBLOCK;
	fcntl(fd, F_SETFL, newOption);
	return oldOption;
}
void TMemter::add_fd(int epoll_fd, int fd) {
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLOUT | EPOLLET | EPOLLERR;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
	set_nonblocking(fd);
}

bool TMemter::writen(int sockfd, const char *buffer, int len) {
	int bytesWrite = 0;
	while(1) {
		bytesWrite = write(sockfd, buffer, len);
		if (bytesWrite == -1) {
			return false;
		} else if (bytesWrite == 0) {
			return false;
		}
		len -= bytesWrite;
		buffer = buffer + bytesWrite;
		if (len <= 0) {
			return true;
		}
	}
}

bool TMemter::read_once(int sockfd, char *buffer, int len) {
	int bytesRead = 0;
	bzero(buffer, len);
	bytesRead = recv(sockfd, buffer, len, 0);
	if (bytesRead == -1) {
		return false;
	} else if (bytesRead == 0) {
		return false;
	}
	return true;
}

void TMemter::close_conn(int epoll_fd, int sockfd) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sockfd, 0);
	close(sockfd);
}
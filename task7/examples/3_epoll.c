#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

int main() 
{
	struct epoll_event event;
	struct epoll_event events[5];
	int epoll_fd = epoll_create1(0);
	int len_rd;
	char buf[4096];
	if (epoll_fd == -1) {
		printf("Epoll create error\n");
		return 1;
	}

	event.events  = EPOLLIN;
	event.data.fd = 0;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event)) {
		printf("Failed to add \n");
		return 2;
	}
/*
	if (close(epoll_fd)) {
		return 3;
	}
*/
	int i;
	int event_count  = epoll_wait(epoll_fd, events, 5 , 5000);
	if (event_count==-1) {
		perror("Epoll wait\n");
		return 4;
	} else {
		printf("events count %d\n", event_count);
		for (i=0;i<event_count;i++){
			printf("Handling fd:%u\n",events[i].data.fd);
			len_rd = read(events[i].data.fd, buf, sizeof(buf));
			buf[len_rd]= '\0';
			printf("%s", buf);
		}
	}
	return 0;
}

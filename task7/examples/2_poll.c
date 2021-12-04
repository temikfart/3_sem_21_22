#include <sys/poll.h>
#include <unistd.h>
#include <stdio.h>

#define TIMEOUT 5

int main() {
    struct pollfd fds[2];
    int ret;

    fds[0].fd = 0;
    fds[0].events = POLLIN;

    fds[1].fd = 1;
    fds[1].events = POLLOUT;

    ret = poll(fds, 1, TIMEOUT * 1000);
    while (1) {
        if (!ret) {
            printf("TIMEOUT\n");
        } else {
            if (fds[0].revents & POLLIN) {
                char str[1024];
                scanf("%s", str);
                printf("%s\n", str);
            }
        
        }
    }
    return 0;
}

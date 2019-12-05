#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "inet_sockets.h"
#include "tlpi_hdr.h"

#define SERVICE    "echo"
#define BUF_SIZE   4096
#define MAX_EVENTS 5

static void grim_reaper(int sig)
{
  int saved_errno;

  saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0) {
    continue;
  }
  errno = saved_errno;
}

static void handle_request(int cfd)
{
  char buf[BUF_SIZE];
  ssize_t num_read;

  while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
    if (write(cfd, buf, num_read) != num_read) {
      printf("write() failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  if (num_read == -1) {
    printf("Error from read(): %s", strerror(errno));
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[])
{
  int sfd_udp, sfd_tcp, cfd_tcp;
  socklen_t len;
  ssize_t num_read;
  struct sockaddr_storage claddr;
  char buf[BUF_SIZE];
  char addr_str[IS_ADDR_STR_LEN];
  int epfd, ready, j;
  struct epoll_event ev;
  struct epoll_event evlist[MAX_EVENTS];
  struct sigaction sa;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = grim_reaper;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    errExit("sigaction");
  }

  sfd_udp = inetBind(SERVICE, SOCK_DGRAM, NULL);
  if (sfd_udp == -1) {
    errExit("inetBind");
  }

  sfd_tcp = inetListen(SERVICE, 10, NULL);
  if (sfd_tcp == -1) {
    errExit("inetListen");
  }

  epfd = epoll_create(MAX_EVENTS);
  if (epfd == -1) {
    errExit("epoll_create");
  }

  ev.events = EPOLLIN;
  ev.data.fd = sfd_udp;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd_udp, &ev) == -1) {
    errExit("epoll_ctl");
  }

  ev.events = EPOLLIN;
  ev.data.fd = sfd_tcp;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd_tcp, &ev) == -1) {
    errExit("epoll_ctl");
  }

  for (; ; ) {
    printf("call epoll_wait()\n");
    ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
    if (ready == -1) {
      if (errno == EINTR) {
        printf("errno is EINTR\n");
        continue;
      }
      else {
        errExit("epoll_wait");
      }
    }

    printf("ready: %d\n", ready);

    for (j = 0; j < ready; ++j) {
      if (evlist[j].data.fd == sfd_udp) {
        printf("UDP echo\n");

        len = sizeof(struct sockaddr_storage);
        num_read = recvfrom(evlist[j].data.fd, buf, BUF_SIZE, 0,
                            (struct sockaddr *)&claddr, &len);
        if (num_read == -1) {
          errExit("recvfrom");
        }

        if (sendto(evlist[j].data.fd, buf, num_read, 0,
                   (struct sockaddr *)&claddr, len)
            != num_read) {
          printf("Error echoing response to %s (%s)\n",
                 inetAddressStr((struct sockaddr *)&claddr, len,
                                addr_str, IS_ADDR_STR_LEN),
                 strerror(errno));
        }
      }
      else if (evlist[j].data.fd == sfd_tcp) {
        printf("TCP echo\n");

        cfd_tcp = accept(sfd_tcp, NULL, NULL);
        if (cfd_tcp == -1) {
          errExit("accept");
        }

        printf("client fd: %d\n", cfd_tcp);

        ev.events = EPOLLIN | EPOLLET; // EPOLLET needed
        ev.data.fd = cfd_tcp;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd_tcp, &ev) == -1) {
          errExit("epoll_ctl");
        }
      }
      else if (evlist[j].events & EPOLLIN) {
        printf("EPOLLIN's fd: %d\n", evlist[j].data.fd);

        switch (fork()) {
        case -1:
          printf("Can't create child (%s)", strerror(errno));
          close(evlist[j].data.fd);
          break; // May be temporary; try next client
        case 0:
          handle_request(evlist[j].data.fd);
          _exit(EXIT_SUCCESS);
        default:
          close(evlist[j].data.fd); // Unneeded copy of connected socket
          break;
        }
      }
    }
  }
}

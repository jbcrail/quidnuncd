#include "qnd.h"

int qn_server_init(struct qn_server *svr)
{
  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  sg_init(1);
  if (sg_drop_privileges() != SG_ERROR_NONE)
    exit(1);

  return 0;
}

int qn_server_listen(struct qn_server *svr, const char *host, const char *port, int backlog)
{
  int flags;
  int listenfd;
  const int option = 1;
  struct addrinfo hints, *res, *reshead;

  // Resolve hostname only for IPv4 addresses
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host, port, &hints, &res) != 0) {
    perror("getaddrinfo");
    return -1;
  }

  reshead = res;

  do {
    listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listenfd < 0) {
      continue;
    }

    /* Reuse listening socket if possible */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0) {
      perror("setsockopt");
      return -1;
    }

    /* Get defined flags for listening socket */
    if ((flags = fcntl(listenfd, F_GETFL, 0)) < 0) {
      perror("fcntl");
      return -1;
    }

    /* Set listening socket as non-blocking */
    if (fcntl(listenfd, F_SETFL, flags | O_NONBLOCK) < 0) {
      perror("fcntl");
      return -1;
    }

    if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0) {
      break;
    }

    close(listenfd);
  } while ((res = res->ai_next) != NULL);

  if (res == NULL) {
    fprintf(stderr, "qn_server_listen error for %s:%s\n", host, port);
    return -1;
  }

  if (listen(listenfd, backlog) < 0) {
    perror("listen");
    return -1;
  }

  freeaddrinfo(reshead);

  svr->sd = listenfd;

  return 0;
}

void qn_server_cleanup(struct qn_server *svr)
{
  struct qn_client *current, *tmp;
  HASH_ITER(hh, svr->clients, current, tmp) {
    close(current->fd);
  }

  sg_shutdown();
  ev_loop_destroy(EV_DEFAULT_UC);
}

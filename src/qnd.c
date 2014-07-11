#include "qnd.h"

struct qnd_context *ctx;

int main(int argc, char *argv[])
{
  struct ev_io w_accept;
  struct ev_loop *loop = ev_default_loop(0);
  int sd;
  struct sockaddr_in addr;
  int addr_len = sizeof(addr);

  ctx = qnd_context_init();

  if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    return -1;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0) {
    perror("bind error");
    return -1;
  }

  if (listen(sd, 2) < 0) {
    perror("listen error");
    return -1;
  }

  ev_io_init(&w_accept, accept_cb, sd, EV_READ);
  ev_io_start(loop, &w_accept);

  ev_loop(loop, 0);

  qnd_context_free(ctx);

  return 0;
}

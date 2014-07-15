#include "qnd.h"

/* Global data structure for server */
struct qn_server server;

int main(int argc, char **argv)
{
  struct ev_signal signal_watcher;
  struct ev_io w_accept;
  struct ev_loop *loop = ev_default_loop(0);

  qn_server_init(&server);
  if (qn_server_listen(&server, DEFAULT_PORT) == -1)
    exit(1);

  ev_signal_init(&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &signal_watcher);

  ev_io_init(&w_accept, accept_cb, server.sd, EV_READ);
  ev_io_start(loop, &w_accept);

  ev_loop(loop, 0);

  ev_signal_stop(loop, &signal_watcher);
  ev_io_stop(loop, &w_accept);
  qn_server_cleanup(&server);

  return 0;
}

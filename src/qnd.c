#include "qnd.h"

/* Global data structure for server */
struct qn_server server;

int main(int argc, char **argv)
{
  struct ev_signal signal_watcher;
  struct ev_io w_accept;
  ev_periodic heartbeat_timer;
  struct ev_loop *loop = ev_default_loop(0);

  int ch;
  double hb_interval = DEFAULT_HEARTBEAT;
  while ((ch = getopt(argc, argv, "i:")) != -1) {
    switch (ch) {
      case 'i':
        hb_interval = atof(optarg);
        break;
      case '?':
      default:
        break;
    }
  }
  argc -= optind;
  argv += optind;

  qn_server_init(&server);
  if (qn_server_listen(&server, DEFAULT_PORT) == -1)
    exit(1);

  ev_signal_init(&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &signal_watcher);

  ev_io_init(&w_accept, accept_cb, server.sd, EV_READ);
  ev_io_start(loop, &w_accept);

  ev_periodic_init(&heartbeat_timer, heartbeat_cb, 0.0, hb_interval, 0);
  ev_periodic_start(loop, &heartbeat_timer);

  ev_loop(loop, 0);

  ev_signal_stop(loop, &signal_watcher);
  ev_io_stop(loop, &w_accept);
  qn_server_cleanup(&server);

  return 0;
}

#include "qnd.h"

/* Global data structure for server */
struct qn_server server;

struct qn_options {
  char *host;
  int port;
  int heartbeat_interval;
};

void usage()
{
  printf(
"Usage: qnd [options]\n"
"\n"
"  --help                       show this help\n"
"  --version                    show version information\n"
"  --host <address>             listen on address (default is 0.0.0.0)\n"
"  --port <int>                 listen on port (default is %d)\n"
"  --heartbeat-interval <int>   issue a heartbeat once every <int> seconds\n",
  DEFAULT_PORT);

  exit(EXIT_SUCCESS);
}

void parse_args(int argc, char **argv, struct qn_options *options)
{
  for (int i = 1; i < argc; i++) {
    int lastarg = (i == argc-1);
    if (!strcasecmp(argv[i], "--help")) {
      usage();
    } else if (!strcasecmp(argv[i],"--version")) {
      printf(QND_VERSION "\n");
      exit(EXIT_SUCCESS);
    } else if (!strcasecmp(argv[i],"--host") && !lastarg) {
      options->host = argv[++i];
    } else if (!strcasecmp(argv[i],"--port") && !lastarg) {
      options->port = strtol(argv[++i], (char **)0, 10);
    } else if (!strcasecmp(argv[i],"--heartbeat-interval") && !lastarg) {
      options->heartbeat_interval = strtol(argv[++i], (char **)0, 10);
    } else {
      fprintf(stderr, "Unknown option: '%s'\n", argv[i]);
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv)
{
  // Define default options
  struct qn_options options;
  options.host = "0.0.0.0";
  options.port = DEFAULT_PORT;
  options.heartbeat_interval = DEFAULT_HEARTBEAT;

  parse_args(argc, argv, &options);

  struct ev_signal signal_watcher;
  struct ev_io w_accept;
  ev_periodic heartbeat_timer;
  struct ev_loop *loop = ev_default_loop(0);

  qn_server_init(&server);
  if (qn_server_listen(&server, options.port) == -1) {
    exit(EXIT_FAILURE);
  }

  ev_signal_init(&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &signal_watcher);

  ev_io_init(&w_accept, accept_cb, server.sd, EV_READ);
  ev_io_start(loop, &w_accept);

  ev_periodic_init(&heartbeat_timer, heartbeat_cb, 0.0, (ev_tstamp)options.heartbeat_interval, 0);
  ev_periodic_start(loop, &heartbeat_timer);

  ev_loop(loop, 0);

  ev_signal_stop(loop, &signal_watcher);
  ev_io_stop(loop, &w_accept);
  qn_server_cleanup(&server);

  return 0;
}

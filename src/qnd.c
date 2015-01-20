#include "qnd.h"

/* Global data structure for server */
struct qn_server server;

struct qn_config {
  char *host;
  char *port;
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
"  --port <int>                 listen on port (default is %s)\n"
"  --heartbeat-interval <int>   issue a heartbeat once every <int> seconds\n",
  DEFAULT_PORT);

  exit(EXIT_SUCCESS);
}

void parse_args(int argc, char **argv, struct qn_config *config)
{
  size_t arglen;
  for (int i = 1; i < argc; i++) {
    int lastarg = (i == argc-1);
    arglen = strlen(argv[i]);
    if (!strncasecmp(argv[i], "--help", arglen)) {
      usage();
    } else if (!strncasecmp(argv[i], "--version", arglen)) {
      printf(QND_VERSION "\n");
      exit(EXIT_SUCCESS);
    } else if (!strncasecmp(argv[i], "--host", arglen) && !lastarg) {
      config->host = argv[++i];
    } else if (!strncasecmp(argv[i], "--port", arglen) && !lastarg) {
      config->port = argv[++i];
    } else if (!strncasecmp(argv[i], "--heartbeat-interval", arglen) && !lastarg) {
      config->heartbeat_interval = strtol(argv[++i], (char **)0, 10);
    } else {
      fprintf(stderr, "Unknown option: '%s'\n", argv[i]);
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv)
{
  // Define default config
  struct qn_config config;
  config.host = DEFAULT_HOST;
  config.port = DEFAULT_PORT;
  config.heartbeat_interval = DEFAULT_HEARTBEAT;

  parse_args(argc, argv, &config);

  struct ev_signal signal_watcher;
  struct ev_io w_accept;
  ev_periodic heartbeat_timer;
  struct ev_loop *loop = ev_default_loop(0);

  qn_server_init(&server);
  if (qn_server_listen(&server, config.host, config.port) == -1) {
    exit(EXIT_FAILURE);
  }

  ev_signal_init(&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &signal_watcher);

  ev_io_init(&w_accept, accept_cb, server.sd, EV_READ);
  ev_io_start(loop, &w_accept);

  ev_periodic_init(&heartbeat_timer, heartbeat_cb, 0.0, (ev_tstamp)config.heartbeat_interval, 0);
  ev_periodic_start(loop, &heartbeat_timer);

  ev_loop(loop, 0);

  ev_signal_stop(loop, &signal_watcher);
  ev_io_stop(loop, &w_accept);
  qn_server_cleanup(&server);
  qn_client_delete_all();

  return 0;
}

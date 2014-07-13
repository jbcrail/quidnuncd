#include "qnd.h"

/* Global context for server */
qnd_context ctx;

int main(int argc, char **argv)
{
  struct ev_signal signal_watcher;
  struct ev_io w_accept;
  struct ev_loop *loop = ev_default_loop(0);

  qnd_context_init(&ctx);
  if (qnd_context_listen(&ctx, DEFAULT_PORT) == -1)
    exit(1);

  ev_signal_init(&signal_watcher, sigint_cb, SIGINT);
  ev_signal_start(loop, &signal_watcher);

  ev_io_init(&w_accept, accept_cb, ctx.sd, EV_READ);
  ev_io_start(loop, &w_accept);

  ev_loop(loop, 0);

  qnd_context_cleanup(&ctx);

  return 0;
}

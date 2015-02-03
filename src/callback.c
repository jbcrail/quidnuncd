#include "qnd.h"

extern struct qn_server server;

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  // Ignore non-readable events
  if ((revents | EV_READ) == 0) {
    return;
  }

  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

  if (client_sd < 0) {
    perror("accept error");
    return;
  }

  struct qn_client *cli = qn_client_new(loop, client_sd);
  ev_io_init(&cli->read_watcher, read_cb, client_sd, EV_READ);
  ev_io_init(&cli->write_watcher, write_cb, client_sd, EV_WRITE);
  ev_io_start(loop, &cli->read_watcher);
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  // Ignore non-readable events
  if ((revents | EV_READ) == 0) {
    return;
  }

  struct qn_client *cli = qn_client_find(watcher->fd);

  if (!qn_client_read(cli)) {
    qn_client_shutdown(cli);
    goto flush;
  }

  while (1) {
    sds request = qn_client_get_request(cli);
    if (request == NULL || strlen(request) == 0) break;
    cli->srv->total_requests++;

    size_t reqlen = strlen(request);
    char *delimiter = strstr(request, " ");
    if (delimiter != NULL) {
      reqlen = delimiter - request;
    }

    if (!strncasecmp(request, "help", reqlen)) {
      cli->wbuf = help_handler(cli);
    } else if (!strncasecmp(request, "ping", reqlen)) {
      cli->wbuf = ping_handler(cli);
    } else if (!strncasecmp(request, "time", reqlen)) {
      cli->wbuf = time_handler(cli);
    } else if (!strncasecmp(request, "info", reqlen)) {
      cli->wbuf = info_handler(cli);
    } else if (!strncasecmp(request, "quit", reqlen)) {
      qn_client_shutdown(cli);
      goto flush;
    } else {
      cli->wbuf = sdscatprintf(cli->wbuf, "error=invalid command: %s\r\n\r\n", request);
    }
  }

flush:
  if (strlen(cli->wbuf) > 0) {
    ev_io_start(loop, &cli->write_watcher);
  } else {
    qn_client_delete(cli);
  }
}

void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  // Ignore non-writeable events
  if ((revents | EV_WRITE) == 0) {
    return;
  }

  struct qn_client *cli = qn_client_find(watcher->fd);

  if (!qn_client_write(cli)) {
    qn_client_delete(cli);
    return;
  }

  if (strlen(cli->wbuf) == 0) {
    if (cli->eof) {
      qn_client_delete(cli);
    } else {
      ev_io_stop(cli->loop, &cli->write_watcher);
    }
  }
}

void shutdown_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
  ev_break(loop, EVBREAK_ALL);
}

void heartbeat_cb(struct ev_loop *loop, ev_periodic *w, int revents)
{
  server.heartbeats++;
}

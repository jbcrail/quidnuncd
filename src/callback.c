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

  qn_client_add(client_sd);
  server.total_clients++;
  server.active_clients++;

  struct ev_io *w_client = (struct ev_io*)malloc(sizeof(struct ev_io));
  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  // Ignore non-readable events
  if ((revents | EV_READ) == 0) {
    return;
  }

  struct qn_client *cli = qn_client_find(watcher->fd);
  ssize_t bytes = qn_client_read(cli);

  if (bytes < 0) {
    perror("read error");
    goto cleanup;
  }

  if (bytes == 0) {
    goto cleanup;
  }

  while (1) {
    sds request = qn_client_get_request(cli);
    if (request == NULL) break;
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
      goto cleanup;
    } else {
      cli->wbuf = sdscatprintf(cli->wbuf, "error=invalid command: %s\r\n\r\n", request);
    }

    if (strstr(cli->wbuf, "\r\n\r\n") != NULL) {
      // TODO: set write callback to send all responses
      bytes = qn_client_write(cli);
      if (bytes < 0) {
        perror("send error");
        goto cleanup;
      }
    }
  }

  return;

cleanup:
  ev_io_stop(loop, watcher);
  free(watcher);

  close(cli->fd);
  qn_client_delete(cli);
  server.active_clients--;
}

void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  // Ignore non-writeable events
  if ((revents | EV_WRITE) == 0) {
    return;
  }
}

void sigint_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
  ev_break(loop, EVBREAK_ALL);
}

void heartbeat_cb(struct ev_loop *loop, ev_periodic *w, int revents)
{
  server.heartbeats++;
}

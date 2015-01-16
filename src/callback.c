#include "qnd.h"

extern struct qn_server server;

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  if (EV_ERROR & revents) {
    perror("invalid event");
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

  struct ev_io *w_client = (struct ev_io*)malloc(sizeof(struct ev_io));
  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  if (EV_ERROR & revents) {
    perror("invalid event");
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

    // Parse requests based on first character for now
    switch (request[0]) {
      case 'p':
        cli->wbuf = ping_handler(cli);
        break;

      case 't':
        cli->wbuf = time_handler(cli);
        break;

      case 'i':
        cli->wbuf = info_handler(cli);
        break;

      case 'q':
        goto cleanup;

      default:
        cli->wbuf = sdscatprintf(cli->wbuf, "error=invalid command: %s\r\n\r\n", request);
        break;
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
}

void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  if (EV_ERROR & revents) {
    perror("invalid event");
    return;
  }
}

void sigint_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
  ev_break(loop, EVBREAK_ALL);
}

void heartbeat_cb(struct ev_loop *loop, ev_periodic *w, int revents)
{
}

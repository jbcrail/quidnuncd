#include "qnd.h"

extern qnd_context ctx;

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
  struct ev_io *w_client = (struct ev_io*)malloc(sizeof(struct ev_io));

  if (EV_ERROR & revents) {
    perror("invalid event");
    return;
  }

  client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

  if (client_sd < 0) {
    perror("accept error");
    return;
  }

  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  if (EV_ERROR & revents) {
    perror("invalid event");
    return;
  }

  ctx.rbuf_idx = recv(watcher->fd, ctx.rbuf, DEFAULT_BUFFER_SIZE, 0);

  if (ctx.rbuf_idx < 0) {
    perror("read error");
    goto cleanup;
  }

  if (ctx.rbuf_idx == 0) {
    ev_io_stop(loop,watcher);
    close(watcher->fd);
    free(watcher);

    perror("remote connection is closing");
    goto cleanup;
  }

  char *pos = strstr(ctx.rbuf, "\r\n");
  if (pos == NULL) return;

  sds request = sdscpylen(sdsempty(), ctx.rbuf, pos-ctx.rbuf+2);
  sds response = sdsempty();

  sdstolower(request);

  // Parse requests based on first character for now
  switch (request[0]) {
    case 'p':
      response = ping_handler(&ctx, request, response);
      break;

    case 't':
      response = time_handler(&ctx, request, response);
      break;

    case 'i':
      response = info_handler(&ctx, request, response);
      break;

    default:
      break;
  }

  if (sdslen(response) > 0) {
    ssize_t bytes = send(watcher->fd, response, sdslen(response), 0);
    if (bytes < 0) {
      perror("send error");
    }
  }

  sdsfree(response);
  sdsfree(request);

cleanup:
  bzero(ctx.rbuf, ctx.rbuf_idx);
  ctx.rbuf_idx = 0;
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

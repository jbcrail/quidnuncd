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
  ssize_t read;

  if (EV_ERROR & revents) {
    perror("invalid event");
    return;
  }

  read = recv(watcher->fd, ctx.rbuf, DEFAULT_BUFFER_SIZE, 0);

  if (read < 0) {
    perror("read error");
    return;
  }

  if (read == 0) {
    ev_io_stop(loop,watcher);
    close(watcher->fd);
    free(watcher);

    perror("remote connection is closing");
    return;
  }

  switch (ctx.rbuf[0]) {
    case 'P':
      printf("PING\n");
      qnd_cmd_ping(&ctx, watcher);
      send(watcher->fd, ctx.wbuf, strlen(ctx.wbuf)+1, 0);
      bzero(ctx.wbuf, strlen(ctx.wbuf));
      break;

    case 'T':
      printf("TIME\n");
      qnd_cmd_time(&ctx, watcher);
      send(watcher->fd, ctx.wbuf, strlen(ctx.wbuf)+1, 0);
      bzero(ctx.wbuf, strlen(ctx.wbuf));
      break;

    case 'I':
      printf("INFO\n");
      qnd_cmd_info(&ctx, watcher);
      send(watcher->fd, ctx.wbuf, strlen(ctx.wbuf)+1, 0);
      bzero(ctx.wbuf, strlen(ctx.wbuf));
      break;

    default:
      break;
  }

  bzero(ctx.rbuf, read);
}

void sigint_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
  ev_break(loop, EVBREAK_ALL);
}

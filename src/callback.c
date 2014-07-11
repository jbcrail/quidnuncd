#include "qnd.h"

extern struct qnd_context *ctx;

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

  read = recv(watcher->fd, ctx->buffer, BUFFER_SIZE, 0);
  char ch = ctx->buffer[0];
  bzero(ctx->buffer, read);

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

  switch (ch) {
    case 'P':
      printf("PING\n");
      qnd_cmd_ping(ctx, watcher);
      send(watcher->fd, ctx->buffer, strlen(ctx->buffer)+1, 0);
      bzero(ctx->buffer, strlen(ctx->buffer));
      break;

    case 'T':
      printf("TIME\n");
      qnd_cmd_time(ctx, watcher);
      send(watcher->fd, ctx->buffer, strlen(ctx->buffer)+1, 0);
      bzero(ctx->buffer, strlen(ctx->buffer));
      break;

    case 'I':
      printf("INFO\n");
      qnd_cmd_info(ctx, watcher);
      send(watcher->fd, ctx->buffer, strlen(ctx->buffer)+1, 0);
      bzero(ctx->buffer, strlen(ctx->buffer));
      break;

    default:
      break;
  }
}

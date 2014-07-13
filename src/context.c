#include "qnd.h"

int qnd_context_init(qnd_context *ctx)
{
  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  sg_init(1);
  if (sg_drop_privileges() != SG_ERROR_NONE)
    exit(1);

  return 0;
}

int qnd_context_listen(qnd_context *ctx, int port)
{
  struct sockaddr_in addr;

  if ((ctx->sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    return -1;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(ctx->sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
    perror("bind error");
    return -1;
  }

  if (listen(ctx->sd, 2) < 0) {
    perror("listen error");
    return -1;
  }

  return 0;
}

// Helper function to simplify writing reply to a client
int qnd_context_write(qnd_context *ctx, const char *fmt, ...)
{
  int rc;
  size_t size = sizeof(ctx->wbuf) - ctx->wbuf_idx;
  va_list args;

  if (size <= 0) {
    return -1;
  }

  va_start(args, fmt);
  rc = vsnprintf(ctx->wbuf+ctx->wbuf_idx, size, fmt, args);
  ctx->wbuf_idx += rc;
  va_end(args);

  return (rc >= size) ? -1 : rc;
}

void qnd_context_cleanup(qnd_context *ctx)
{
  sg_shutdown();
}

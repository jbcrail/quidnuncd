#include "qnd.h"

int qnd_context_init(qnd_context *ctx)
{
  sg_init(1);
  if (sg_drop_privileges() != SG_ERROR_NONE)
    exit(1);

  ctx->host_stats = sg_get_host_info(NULL);
  if (ctx->host_stats == NULL)
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

void qnd_context_cleanup(qnd_context *ctx)
{
  sg_shutdown();

  free(ctx->buffer);
  free(ctx);
}

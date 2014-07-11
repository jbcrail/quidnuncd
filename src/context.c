#include "qnd.h"

struct qnd_context *
qnd_context_init()
{
  struct qnd_context *ctx = (struct qnd_context*)malloc(sizeof(struct qnd_context));
  ctx->buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);

  sg_init(1);

  if (sg_drop_privileges() != SG_ERROR_NONE)
    exit(1);

  ctx->host_stats = sg_get_host_info(NULL);
  if (ctx->host_stats == NULL)
    exit(1);

  return ctx;
}

void
qnd_context_free(struct qnd_context *ctx)
{
  sg_shutdown();

  free(ctx->buffer);
  free(ctx);
}

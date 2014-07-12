#include "qnd.h"

void qnd_cmd_ping(qnd_context *ctx, struct ev_io *watcher)
{
  sprintf(ctx->buffer, "+PONG\r\n");
}

void qnd_cmd_time(qnd_context *ctx, struct ev_io *watcher)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sprintf(ctx->buffer, "*2\r\n:%ld\r\n:%d\r\n", tv.tv_sec, tv.tv_usec);
}

void qnd_cmd_info(qnd_context *ctx, struct ev_io *watcher)
{
  char *format = "*9\r\n"
                 "$0\r\nos_name:%s\r\n"
                 "$0\r\nos_release:%s\r\n"
                 "$0\r\nos_platform:%s\r\n"
                 "$0\r\nhostname:%s\r\n"
                 "$0\r\nuptime_in_seconds:%lld\r\n"
                 "$0\r\nuptime_in_days:%lld\r\n"
                 "$0\r\ncpus_max:%u\r\n"
                 "$0\r\ncpus_available:%u\r\n"
                 "$0\r\narch_bits:%u\r\n";

  sprintf(ctx->buffer, format,
    ctx->host_stats->os_name,
    ctx->host_stats->os_release,
    ctx->host_stats->platform,
    ctx->host_stats->hostname,
    (long long)ctx->host_stats->uptime,
    (long long)ctx->host_stats->uptime/60/60/24,
    ctx->host_stats->maxcpus,
    ctx->host_stats->ncpus,
    ctx->host_stats->bitwidth);
}

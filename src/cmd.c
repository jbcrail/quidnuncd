#include "qnd.h"

void qnd_cmd_ping(qnd_context *ctx, struct ev_io *watcher)
{
  qnd_context_write(ctx, "pong\r\n");
}

void write_timestamp(qnd_context *ctx)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  qnd_context_write(ctx, "time.secs=%ju\r\ntime.usecs=%ju\r\n", (uintmax_t)tv.tv_sec, (uintmax_t)tv.tv_usec);
}

void qnd_cmd_time(qnd_context *ctx, struct ev_io *watcher)
{
  write_timestamp(ctx);
}

void qnd_cmd_info(qnd_context *ctx, struct ev_io *watcher)
{
  if (strcasestr(ctx->rbuf, "fs") != NULL) {
    ctx->fs_stats = sg_get_fs_stats(&ctx->fs_size);
    if (ctx->fs_stats == NULL) return;

    for (int i = 0; i < ctx->fs_size; i++) {
      qnd_context_write(ctx, "fs.%d.avail=%llu\r\n", i, ctx->fs_stats[i].avail);
      qnd_context_write(ctx, "fs.%d.avail_blocks=%llu\r\n", i, ctx->fs_stats[i].avail_blocks);
      qnd_context_write(ctx, "fs.%d.avail_inodes=%llu\r\n", i, ctx->fs_stats[i].avail_inodes);
      qnd_context_write(ctx, "fs.%d.block_size=%llu\r\n", i, ctx->fs_stats[i].block_size);
      qnd_context_write(ctx, "fs.%d.device=%s\r\n", i, ctx->fs_stats[i].device_name);
      qnd_context_write(ctx, "fs.%d.free_blocks=%llu\r\n", i, ctx->fs_stats[i].free_blocks);
      qnd_context_write(ctx, "fs.%d.free_inodes=%llu\r\n", i, ctx->fs_stats[i].free_inodes);
      qnd_context_write(ctx, "fs.%d.fs_type=%s\r\n", i, ctx->fs_stats[i].fs_type);
      qnd_context_write(ctx, "fs.%d.io_size=%llu\r\n", i, ctx->fs_stats[i].io_size);
      qnd_context_write(ctx, "fs.%d.mount=%s\r\n", i, ctx->fs_stats[i].mnt_point);
      qnd_context_write(ctx, "fs.%d.size=%llu\r\n", i, ctx->fs_stats[i].size);
      qnd_context_write(ctx, "fs.%d.total_blocks=%llu\r\n", i, ctx->fs_stats[i].total_blocks);
      qnd_context_write(ctx, "fs.%d.total_inodes=%llu\r\n", i, ctx->fs_stats[i].total_inodes);
      qnd_context_write(ctx, "fs.%d.used=%llu\r\n", i, ctx->fs_stats[i].used);
      qnd_context_write(ctx, "fs.%d.used_blocks=%llu\r\n", i, ctx->fs_stats[i].used_blocks);
      qnd_context_write(ctx, "fs.%d.used_inodes=%llu\r\n", i, ctx->fs_stats[i].used_inodes);
    }
  } else if (strcasestr(ctx->rbuf, "load") != NULL) {
    ctx->load_stats = sg_get_load_stats(NULL);
    if (ctx->load_stats == NULL) return;

    qnd_context_write(ctx, "load.min1=%0.2f\r\n", ctx->load_stats->min1);
    qnd_context_write(ctx, "load.min5=%0.2f\r\n", ctx->load_stats->min5);
    qnd_context_write(ctx, "load.min15=%0.2f\r\n", ctx->load_stats->min15);
  } else if (strcasestr(ctx->rbuf, "memory") != NULL) {
    ctx->mem_stats = sg_get_mem_stats(NULL);
    if (ctx->mem_stats == NULL) return;

    qnd_context_write(ctx, "memory.total=%llu\r\n", ctx->mem_stats->total);
    qnd_context_write(ctx, "memory.used=%llu\r\n", ctx->mem_stats->used);
    qnd_context_write(ctx, "memory.cache=%llu\r\n", ctx->mem_stats->cache);
    qnd_context_write(ctx, "memory.free=%llu\r\n", ctx->mem_stats->free);
  } else {
    ctx->host_stats = sg_get_host_info(NULL);
    if (ctx->host_stats == NULL) return;

    qnd_context_write(ctx, "server.os_name=%s\r\n", ctx->host_stats->os_name);
    qnd_context_write(ctx, "server.os_release=%s\r\n", ctx->host_stats->os_release);
    qnd_context_write(ctx, "server.os_platform=%s\r\n", ctx->host_stats->platform);
    qnd_context_write(ctx, "server.hostname=%s\r\n", ctx->host_stats->hostname);
    qnd_context_write(ctx, "server.uptime_in_seconds=%lld\r\n", (uint64_t)ctx->host_stats->uptime);
    qnd_context_write(ctx, "server.uptime_in_days=%lld\r\n", (uint64_t)ctx->host_stats->uptime/60/60/24);
    qnd_context_write(ctx, "server.cpu_max=%u\r\n", ctx->host_stats->maxcpus);
    qnd_context_write(ctx, "server.cpu_available=%u\r\n", ctx->host_stats->ncpus);
    qnd_context_write(ctx, "server.arch_bits=%u\r\n", ctx->host_stats->bitwidth);
  }

  write_timestamp(ctx);
}

#include "qnd.h"

sds write_timestamp(sds s)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return sdscatprintf(s, "time.secs=%ju\r\ntime.usecs=%ju\r\n", (uintmax_t)tv.tv_sec, (uintmax_t)tv.tv_usec);
}

sds ping_handler(qnd_context *ctx, sds request, sds response)
{
  return sdscatprintf(response, "pong\r\n");
}

sds time_handler(qnd_context *ctx, sds request, sds response)
{
  return write_timestamp(response);
}

sds info_handler(qnd_context *ctx, sds request, sds response)
{
  int argc;
  sds *args = sdssplitargs(request, &argc);

  if (argc == 1) {
    ctx->host_stats = sg_get_host_info(NULL);
    if (ctx->host_stats == NULL) return response;

    response = sdscatprintf(response, "server.os_name=%s\r\n", ctx->host_stats->os_name);
    response = sdscatprintf(response, "server.os_release=%s\r\n", ctx->host_stats->os_release);
    response = sdscatprintf(response, "server.os_platform=%s\r\n", ctx->host_stats->platform);
    response = sdscatprintf(response, "server.hostname=%s\r\n", ctx->host_stats->hostname);
    response = sdscatprintf(response, "server.uptime_in_seconds=%lld\r\n", (uint64_t)ctx->host_stats->uptime);
    response = sdscatprintf(response, "server.uptime_in_days=%lld\r\n", (uint64_t)ctx->host_stats->uptime/60/60/24);
    response = sdscatprintf(response, "server.cpu_max=%u\r\n", ctx->host_stats->maxcpus);
    response = sdscatprintf(response, "server.cpu_available=%u\r\n", ctx->host_stats->ncpus);
    response = sdscatprintf(response, "server.arch_bits=%u\r\n", ctx->host_stats->bitwidth);

    goto cleanup;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp(args[i], "fs") == 0) {
      ctx->fs_stats = sg_get_fs_stats(&ctx->fs_size);
      if (ctx->fs_stats == NULL) return response;

      for (int j = 0; j < ctx->fs_size; j++) {
        response = sdscatprintf(response, "fs.%d.avail=%llu\r\n", j, ctx->fs_stats[j].avail);
        response = sdscatprintf(response, "fs.%d.avail_blocks=%llu\r\n", j, ctx->fs_stats[j].avail_blocks);
        response = sdscatprintf(response, "fs.%d.avail_inodes=%llu\r\n", j, ctx->fs_stats[j].avail_inodes);
        response = sdscatprintf(response, "fs.%d.block_size=%llu\r\n", j, ctx->fs_stats[j].block_size);
        response = sdscatprintf(response, "fs.%d.device=%s\r\n", j, ctx->fs_stats[j].device_name);
        response = sdscatprintf(response, "fs.%d.free_blocks=%llu\r\n", j, ctx->fs_stats[j].free_blocks);
        response = sdscatprintf(response, "fs.%d.free_inodes=%llu\r\n", j, ctx->fs_stats[j].free_inodes);
        response = sdscatprintf(response, "fs.%d.fs_type=%s\r\n", j, ctx->fs_stats[j].fs_type);
        response = sdscatprintf(response, "fs.%d.io_size=%llu\r\n", j, ctx->fs_stats[j].io_size);
        response = sdscatprintf(response, "fs.%d.mount=%s\r\n", j, ctx->fs_stats[j].mnt_point);
        response = sdscatprintf(response, "fs.%d.size=%llu\r\n", j, ctx->fs_stats[j].size);
        response = sdscatprintf(response, "fs.%d.total_blocks=%llu\r\n", j, ctx->fs_stats[j].total_blocks);
        response = sdscatprintf(response, "fs.%d.total_inodes=%llu\r\n", j, ctx->fs_stats[j].total_inodes);
        response = sdscatprintf(response, "fs.%d.used=%llu\r\n", j, ctx->fs_stats[j].used);
        response = sdscatprintf(response, "fs.%d.used_blocks=%llu\r\n", j, ctx->fs_stats[j].used_blocks);
        response = sdscatprintf(response, "fs.%d.used_inodes=%llu\r\n", j, ctx->fs_stats[j].used_inodes);
      }
    } else if (strcmp(args[i], "load") == 0) {
      ctx->load_stats = sg_get_load_stats(NULL);
      if (ctx->load_stats == NULL) return response;

      response = sdscatprintf(response, "load.min1=%0.2f\r\n", ctx->load_stats->min1);
      response = sdscatprintf(response, "load.min5=%0.2f\r\n", ctx->load_stats->min5);
      response = sdscatprintf(response, "load.min15=%0.2f\r\n", ctx->load_stats->min15);
    } else if (strcmp(args[i], "memory") == 0) {
      ctx->mem_stats = sg_get_mem_stats(NULL);
      if (ctx->mem_stats == NULL) return response;

      response = sdscatprintf(response, "memory.total=%llu\r\n", ctx->mem_stats->total);
      response = sdscatprintf(response, "memory.used=%llu\r\n", ctx->mem_stats->used);
      response = sdscatprintf(response, "memory.cache=%llu\r\n", ctx->mem_stats->cache);
      response = sdscatprintf(response, "memory.free=%llu\r\n", ctx->mem_stats->free);
    }
  }

cleanup:
  sdsfreesplitres(args, argc);

  return write_timestamp(response);
}

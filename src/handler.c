#include "qnd.h"

sds write_timestamp(sds s)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return sdscatprintf(s, "time.secs=%ju\r\ntime.usecs=%ju\r\n\r\n", (uintmax_t)tv.tv_sec, (uintmax_t)tv.tv_usec);
}

sds ping_handler(struct qn_client *c)
{
  return sdscatprintf(c->wbuf, "pong\r\n\r\n");
}

sds time_handler(struct qn_client *c)
{
  return write_timestamp(c->wbuf);
}

sds info_handler(struct qn_client *c)
{
  int argc;
  sds *args = sdssplitargs(c->request, &argc);

  if (argc == 1) {
    c->srv->host_stats = sg_get_host_info(NULL);
    if (c->srv->host_stats == NULL) return c->wbuf;

    c->wbuf = sdscatprintf(c->wbuf, "server.os_name=%s\r\n", c->srv->host_stats->os_name);
    c->wbuf = sdscatprintf(c->wbuf, "server.os_release=%s\r\n", c->srv->host_stats->os_release);
    c->wbuf = sdscatprintf(c->wbuf, "server.os_platform=%s\r\n", c->srv->host_stats->platform);
    c->wbuf = sdscatprintf(c->wbuf, "server.hostname=%s\r\n", c->srv->host_stats->hostname);
    c->wbuf = sdscatprintf(c->wbuf, "server.uptime_in_seconds=%lld\r\n", (uint64_t)c->srv->host_stats->uptime);
    c->wbuf = sdscatprintf(c->wbuf, "server.uptime_in_days=%lld\r\n", (uint64_t)c->srv->host_stats->uptime/60/60/24);
    c->wbuf = sdscatprintf(c->wbuf, "server.cpu_max=%u\r\n", c->srv->host_stats->maxcpus);
    c->wbuf = sdscatprintf(c->wbuf, "server.cpu_available=%u\r\n", c->srv->host_stats->ncpus);
    c->wbuf = sdscatprintf(c->wbuf, "server.arch_bits=%u\r\n", c->srv->host_stats->bitwidth);

    goto cleanup;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp(args[i], "fs") == 0) {
      c->srv->fs_stats = sg_get_fs_stats(&c->srv->fs_size);
      if (c->srv->fs_stats == NULL) return c->wbuf;

      for (int j = 0; j < c->srv->fs_size; j++) {
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.avail=%llu\r\n", j, c->srv->fs_stats[j].avail);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.avail_blocks=%llu\r\n", j, c->srv->fs_stats[j].avail_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.avail_inodes=%llu\r\n", j, c->srv->fs_stats[j].avail_inodes);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.block_size=%llu\r\n", j, c->srv->fs_stats[j].block_size);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.device=%s\r\n", j, c->srv->fs_stats[j].device_name);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.free_blocks=%llu\r\n", j, c->srv->fs_stats[j].free_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.free_inodes=%llu\r\n", j, c->srv->fs_stats[j].free_inodes);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.fs_type=%s\r\n", j, c->srv->fs_stats[j].fs_type);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.io_size=%llu\r\n", j, c->srv->fs_stats[j].io_size);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.mount=%s\r\n", j, c->srv->fs_stats[j].mnt_point);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.size=%llu\r\n", j, c->srv->fs_stats[j].size);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.total_blocks=%llu\r\n", j, c->srv->fs_stats[j].total_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.total_inodes=%llu\r\n", j, c->srv->fs_stats[j].total_inodes);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.used=%llu\r\n", j, c->srv->fs_stats[j].used);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.used_blocks=%llu\r\n", j, c->srv->fs_stats[j].used_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.used_inodes=%llu\r\n", j, c->srv->fs_stats[j].used_inodes);
      }
    } else if (strcmp(args[i], "load") == 0) {
      c->srv->load_stats = sg_get_load_stats(NULL);
      if (c->srv->load_stats == NULL) return c->wbuf;

      c->wbuf = sdscatprintf(c->wbuf, "load.min1=%0.2f\r\n", c->srv->load_stats->min1);
      c->wbuf = sdscatprintf(c->wbuf, "load.min5=%0.2f\r\n", c->srv->load_stats->min5);
      c->wbuf = sdscatprintf(c->wbuf, "load.min15=%0.2f\r\n", c->srv->load_stats->min15);
    } else if (strcmp(args[i], "memory") == 0) {
      c->srv->mem_stats = sg_get_mem_stats(NULL);
      if (c->srv->mem_stats == NULL) return c->wbuf;

      c->wbuf = sdscatprintf(c->wbuf, "memory.total=%llu\r\n", c->srv->mem_stats->total);
      c->wbuf = sdscatprintf(c->wbuf, "memory.used=%llu\r\n", c->srv->mem_stats->used);
      c->wbuf = sdscatprintf(c->wbuf, "memory.cache=%llu\r\n", c->srv->mem_stats->cache);
      c->wbuf = sdscatprintf(c->wbuf, "memory.free=%llu\r\n", c->srv->mem_stats->free);
    }
  }

cleanup:
  sdsfreesplitres(args, argc);

  return write_timestamp(c->wbuf);
}

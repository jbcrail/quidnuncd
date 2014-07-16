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
    sg_host_info *host_stats = sg_get_host_info(NULL);
    if (host_stats == NULL) return c->wbuf;

    c->wbuf = sdscatprintf(c->wbuf,
      "server.os_name=%s\r\n"
      "server.os_release=%s\r\n"
      "server.os_platform=%s\r\n"
      "server.hostname=%s\r\n"
      "server.uptime_in_seconds=%lld\r\n"
      "server.uptime_in_days=%lld\r\n"
      "server.cpu_max=%u\r\n"
      "server.cpu_available=%u\r\n"
      "server.arch_bits=%u\r\n",
      host_stats->os_name,
      host_stats->os_release,
      host_stats->platform,
      host_stats->hostname,
      (uint64_t)host_stats->uptime,
      (uint64_t)host_stats->uptime/60/60/24,
      host_stats->maxcpus,
      host_stats->ncpus,
      host_stats->bitwidth);

    goto cleanup;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp(args[i], "fs") == 0) {
      size_t fs_size;
      sg_fs_stats *fs_stats = sg_get_fs_stats(&fs_size);
      if (fs_stats == NULL) return c->wbuf;

      for (int j = 0; j < fs_size; j++) {
        c->wbuf = sdscatprintf(c->wbuf,
          "fs.%d.avail=%llu\r\n"
          "fs.%d.avail_blocks=%llu\r\n"
          "fs.%d.avail_inodes=%llu\r\n"
          "fs.%d.block_size=%llu\r\n"
          "fs.%d.device=%s\r\n"
          "fs.%d.free_blocks=%llu\r\n"
          "fs.%d.free_inodes=%llu\r\n"
          "fs.%d.fs_type=%s\r\n"
          "fs.%d.io_size=%llu\r\n"
          "fs.%d.mount=%s\r\n"
          "fs.%d.size=%llu\r\n"
          "fs.%d.total_blocks=%llu\r\n"
          "fs.%d.total_inodes=%llu\r\n"
          "fs.%d.used=%llu\r\n"
          "fs.%d.used_blocks=%llu\r\n"
          "fs.%d.used_inodes=%llu\r\n",
          j, fs_stats[j].avail,
          j, fs_stats[j].avail_blocks,
          j, fs_stats[j].avail_inodes,
          j, fs_stats[j].block_size,
          j, fs_stats[j].device_name,
          j, fs_stats[j].free_blocks,
          j, fs_stats[j].free_inodes,
          j, fs_stats[j].fs_type,
          j, fs_stats[j].io_size,
          j, fs_stats[j].mnt_point,
          j, fs_stats[j].size,
          j, fs_stats[j].total_blocks,
          j, fs_stats[j].total_inodes,
          j, fs_stats[j].used,
          j, fs_stats[j].used_blocks,
          j, fs_stats[j].used_inodes);
      }
    } else if (strcmp(args[i], "load") == 0) {
      sg_load_stats *load_stats = sg_get_load_stats(NULL);
      if (load_stats == NULL) return c->wbuf;

      c->wbuf = sdscatprintf(c->wbuf,
        "load.min1=%0.2f\r\n"
        "load.min5=%0.2f\r\n"
        "load.min15=%0.2f\r\n",
        load_stats->min1,
        load_stats->min5,
        load_stats->min15);
    } else if (strcmp(args[i], "memory") == 0) {
      sg_mem_stats *mem_stats = sg_get_mem_stats(NULL);
      if (mem_stats == NULL) return c->wbuf;

      c->wbuf = sdscatprintf(c->wbuf,
        "memory.total=%llu\r\n"
        "memory.used=%llu\r\n"
        "memory.cache=%llu\r\n"
        "memory.free=%llu\r\n",
        mem_stats->total,
        mem_stats->used,
        mem_stats->cache,
        mem_stats->free);
    } else if (strcmp(args[i], "network") == 0) {
      size_t net_io_size;
      sg_network_io_stats *net_io_stats = sg_get_network_io_stats(&net_io_size);
      if (net_io_stats == NULL) return c->wbuf;

      for (int j = 0; j < net_io_size; j++) {
        char *name = net_io_stats[j].interface_name;

        c->wbuf = sdscatprintf(c->wbuf,
          "net.%s.tx=%llu\r\n"
          "net.%s.rx=%llu\r\n"
          "net.%s.ipackets=%llu\r\n"
          "net.%s.opackets=%llu\r\n"
          "net.%s.ierrors=%llu\r\n"
          "net.%s.oerrors=%llu\r\n"
          "net.%s.collisions=%llu\r\n",
          name, net_io_stats[j].tx,
          name, net_io_stats[j].rx,
          name, net_io_stats[j].ipackets,
          name, net_io_stats[j].opackets,
          name, net_io_stats[j].ierrors,
          name, net_io_stats[j].oerrors,
          name, net_io_stats[j].collisions);
      }
    }
  }

cleanup:
  sdsfreesplitres(args, argc);

  return write_timestamp(c->wbuf);
}

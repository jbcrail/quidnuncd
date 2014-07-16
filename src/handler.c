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

    c->wbuf = sdscatprintf(c->wbuf, "server.os_name=%s\r\n", host_stats->os_name);
    c->wbuf = sdscatprintf(c->wbuf, "server.os_release=%s\r\n", host_stats->os_release);
    c->wbuf = sdscatprintf(c->wbuf, "server.os_platform=%s\r\n", host_stats->platform);
    c->wbuf = sdscatprintf(c->wbuf, "server.hostname=%s\r\n", host_stats->hostname);
    c->wbuf = sdscatprintf(c->wbuf, "server.uptime_in_seconds=%lld\r\n", (uint64_t)host_stats->uptime);
    c->wbuf = sdscatprintf(c->wbuf, "server.uptime_in_days=%lld\r\n", (uint64_t)host_stats->uptime/60/60/24);
    c->wbuf = sdscatprintf(c->wbuf, "server.cpu_max=%u\r\n", host_stats->maxcpus);
    c->wbuf = sdscatprintf(c->wbuf, "server.cpu_available=%u\r\n", host_stats->ncpus);
    c->wbuf = sdscatprintf(c->wbuf, "server.arch_bits=%u\r\n", host_stats->bitwidth);

    goto cleanup;
  }

  for (int i = 1; i < argc; i++) {
    if (strcmp(args[i], "fs") == 0) {
      size_t fs_size;
      sg_fs_stats *fs_stats = sg_get_fs_stats(&fs_size);
      if (fs_stats == NULL) return c->wbuf;

      for (int j = 0; j < fs_size; j++) {
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.avail=%llu\r\n", j, fs_stats[j].avail);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.avail_blocks=%llu\r\n", j, fs_stats[j].avail_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.avail_inodes=%llu\r\n", j, fs_stats[j].avail_inodes);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.block_size=%llu\r\n", j, fs_stats[j].block_size);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.device=%s\r\n", j, fs_stats[j].device_name);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.free_blocks=%llu\r\n", j, fs_stats[j].free_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.free_inodes=%llu\r\n", j, fs_stats[j].free_inodes);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.fs_type=%s\r\n", j, fs_stats[j].fs_type);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.io_size=%llu\r\n", j, fs_stats[j].io_size);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.mount=%s\r\n", j, fs_stats[j].mnt_point);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.size=%llu\r\n", j, fs_stats[j].size);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.total_blocks=%llu\r\n", j, fs_stats[j].total_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.total_inodes=%llu\r\n", j, fs_stats[j].total_inodes);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.used=%llu\r\n", j, fs_stats[j].used);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.used_blocks=%llu\r\n", j, fs_stats[j].used_blocks);
        c->wbuf = sdscatprintf(c->wbuf, "fs.%d.used_inodes=%llu\r\n", j, fs_stats[j].used_inodes);
      }
    } else if (strcmp(args[i], "load") == 0) {
      sg_load_stats *load_stats = sg_get_load_stats(NULL);
      if (load_stats == NULL) return c->wbuf;

      c->wbuf = sdscatprintf(c->wbuf, "load.min1=%0.2f\r\n", load_stats->min1);
      c->wbuf = sdscatprintf(c->wbuf, "load.min5=%0.2f\r\n", load_stats->min5);
      c->wbuf = sdscatprintf(c->wbuf, "load.min15=%0.2f\r\n", load_stats->min15);
    } else if (strcmp(args[i], "memory") == 0) {
      sg_mem_stats *mem_stats = sg_get_mem_stats(NULL);
      if (mem_stats == NULL) return c->wbuf;

      c->wbuf = sdscatprintf(c->wbuf, "memory.total=%llu\r\n", mem_stats->total);
      c->wbuf = sdscatprintf(c->wbuf, "memory.used=%llu\r\n", mem_stats->used);
      c->wbuf = sdscatprintf(c->wbuf, "memory.cache=%llu\r\n", mem_stats->cache);
      c->wbuf = sdscatprintf(c->wbuf, "memory.free=%llu\r\n", mem_stats->free);
    } else if (strcmp(args[i], "network") == 0) {
      size_t net_io_size;
      sg_network_io_stats *net_io_stats = sg_get_network_io_stats(&net_io_size);
      if (net_io_stats == NULL) return c->wbuf;

      for (int j = 0; j < net_io_size; j++) {
        char *name = net_io_stats[j].interface_name;

        c->wbuf = sdscatprintf(c->wbuf, "net.%s.tx=%llu\r\n", name, net_io_stats[j].tx);
        c->wbuf = sdscatprintf(c->wbuf, "net.%s.rx=%llu\r\n", name, net_io_stats[j].rx);
        c->wbuf = sdscatprintf(c->wbuf, "net.%s.ipackets=%llu\r\n", name, net_io_stats[j].ipackets);
        c->wbuf = sdscatprintf(c->wbuf, "net.%s.opackets=%llu\r\n", name, net_io_stats[j].opackets);
        c->wbuf = sdscatprintf(c->wbuf, "net.%s.ierrors=%llu\r\n", name, net_io_stats[j].ierrors);
        c->wbuf = sdscatprintf(c->wbuf, "net.%s.oerrors=%llu\r\n", name, net_io_stats[j].oerrors);
        c->wbuf = sdscatprintf(c->wbuf, "net.%s.collisions=%llu\r\n", name, net_io_stats[j].collisions);
      }
    }
  }

cleanup:
  sdsfreesplitres(args, argc);

  return write_timestamp(c->wbuf);
}

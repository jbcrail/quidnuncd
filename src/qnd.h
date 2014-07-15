#ifndef __QND_H__
#define __QND_H__

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ev.h"
#include "sds.h"
#include "statgrab.h"
#include "uthash.h"

#define DEFAULT_PORT        3230
#define DEFAULT_BUFFER_SIZE 16*1024

struct qn_server {
  /* Networking */
  int port;                                 /* TCP listening port */
  int sd;                                   /* TCP socket file descriptor */

  /* Statistics */
  size_t fs_size;
  size_t net_io_size;
  size_t net_iface_size;

  sg_fs_stats *fs_stats;                    /* Filesystem statistics */
  sg_host_info *host_stats;                 /* Host statistics */
  sg_load_stats *load_stats;                /* Load statistics */
  sg_mem_stats *mem_stats;                  /* Memory statistics */
  sg_network_io_stats *net_io_stats;        /* Network I/O statistics */

  /* Miscellaneous */
  struct qn_client *clients;
};

struct qn_client {
  int fd;
  sds rbuf;
  sds wbuf;
  sds request;
  sds response;
  struct qn_server *srv;
  UT_hash_handle hh;
};

int qn_server_init(struct qn_server *svr);
int qn_server_listen(struct qn_server *svr, int port);
void qn_server_cleanup(struct qn_server *svr);

void qn_client_add(int fd);
struct qn_client *qn_client_find(int fd);
sds qn_client_get_request(struct qn_client *c);
ssize_t qn_client_read(struct qn_client *c);
ssize_t qn_client_write(struct qn_client *c);
void qn_client_delete(struct qn_client *c);
void qn_client_delete_all();

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void sigint_cb(struct ev_loop *loop, struct ev_signal *watcher, int revents);

sds ping_handler(struct qn_client *c);
sds time_handler(struct qn_client *c);
sds info_handler(struct qn_client *c);

#endif

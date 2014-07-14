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

#define DEFAULT_PORT        3230
#define DEFAULT_BUFFER_SIZE 16*1024

typedef struct {
  /* Networking */
  int port;                         /* TCP listening port */
  int sd;                           /* TCP socket file descriptor */

  /* Statistics */
  size_t fs_size;
  sg_fs_stats *fs_stats;            /* Filesystem statistics */
  sg_host_info *host_stats;         /* Host statistics */
  sg_load_stats *load_stats;        /* Load statistics */
  sg_mem_stats *mem_stats;          /* Memory statistics */

  /* Miscellaneous */
  char rbuf[DEFAULT_BUFFER_SIZE];   /* Scratchpad for incoming requests */
  ssize_t rbuf_idx;                 /* Current insertion point for read buffer */

  char wbuf[DEFAULT_BUFFER_SIZE];   /* Scratchpad for outgoing requests */
  ssize_t wbuf_idx;                 /* Current insertion point for write buffer */
} qnd_context;

int qnd_context_init(qnd_context *ctx);
int qnd_context_listen(qnd_context *ctx, int port);
void qnd_context_cleanup(qnd_context *ctx);

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void sigint_cb(struct ev_loop *loop, struct ev_signal *watcher, int revents);

sds ping_handler(qnd_context *ctx, sds request, sds response);
sds time_handler(qnd_context *ctx, sds request, sds response);
sds info_handler(qnd_context *ctx, sds request, sds response);

#endif

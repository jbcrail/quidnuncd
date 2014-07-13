#ifndef __QND_H__
#define __QND_H__

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
  char wbuf[DEFAULT_BUFFER_SIZE];   /* Scratchpad for outgoing requests */
  size_t wbuf_idx;                  /* Current insertion point for write buffer */
} qnd_context;

int qnd_context_init(qnd_context *ctx);
int qnd_context_listen(qnd_context *ctx, int port);
int qnd_context_write(qnd_context *ctx, const char *fmt, ...);
void qnd_context_cleanup(qnd_context *ctx);

void qnd_cmd_ping(qnd_context *ctx, struct ev_io *watcher);
void qnd_cmd_time(qnd_context *ctx, struct ev_io *watcher);
void qnd_cmd_info(qnd_context *ctx, struct ev_io *watcher);

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void sigint_cb(struct ev_loop *loop, struct ev_signal *watcher, int revents);

#endif

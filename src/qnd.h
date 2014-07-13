#ifndef __QND_H__
#define __QND_H__

#include <signal.h>
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
#define DEFAULT_BUFFER_SIZE 1024

typedef struct {
  /* Networking */
  int port;                         /* TCP listening port */
  int sd;                           /* TCP socket file descriptor */

  /* Statistics */
  sg_host_info *host_stats;         /* Host statistics */

  /* Miscellaneous */
  char buffer[DEFAULT_BUFFER_SIZE]; /* Scratchpad for incoming requests */
} qnd_context;

int qnd_context_init(qnd_context *ctx);
int qnd_context_listen(qnd_context *ctx, int port);
void qnd_context_cleanup(qnd_context *ctx);

void qnd_cmd_ping(qnd_context *ctx, struct ev_io *watcher);
void qnd_cmd_time(qnd_context *ctx, struct ev_io *watcher);
void qnd_cmd_info(qnd_context *ctx, struct ev_io *watcher);

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void sigint_cb(struct ev_loop *loop, struct ev_signal *watcher, int revents);

#endif

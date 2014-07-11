#ifndef __QND_H__
#define __QND_H__

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "ev.h"
#include "statgrab.h"

#define PORT        3230
#define BUFFER_SIZE 1024

struct qnd_context {
  sg_host_info *host_stats;
  char *buffer;
};

struct qnd_context* qnd_context_init();
void                qnd_context_free(struct qnd_context *ctx);

void                qnd_cmd_ping(struct qnd_context *ctx, struct ev_io *watcher);
void                qnd_cmd_time(struct qnd_context *ctx, struct ev_io *watcher);
void                qnd_cmd_info(struct qnd_context *ctx, struct ev_io *watcher);

void                accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void                read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

#endif

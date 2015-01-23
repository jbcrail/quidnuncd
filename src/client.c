#include "qnd.h"

extern struct qn_server server;

struct qn_client *qn_client_new(struct ev_loop *loop, int fd)
{
  struct qn_client *c;

  HASH_FIND_INT(server.clients, &fd, c);
  if (c == NULL) {
    c = (struct qn_client *)malloc(sizeof(struct qn_client));
    c->fd = fd;
    c->rbuf = sdsempty();
    c->wbuf = sdsempty();
    c->request = sdsempty();
    c->loop = loop;
    c->srv = &server;
    HASH_ADD_INT(server.clients, fd, c);
    c->srv->total_clients++;
    c->srv->active_clients++;
  }
  return c;
}

struct qn_client *qn_client_find(int fd)
{
  struct qn_client *c;

  HASH_FIND_INT(server.clients, &fd, c);
  return c;
}

sds qn_client_get_request(struct qn_client *c)
{
  sdsclear(c->request);

  char *pos = strstr(c->rbuf, "\r\n");
  if (pos == NULL) return NULL;

  // copy everything but the carriage return and line feed
  c->request = sdscpylen(c->request, c->rbuf, pos - c->rbuf);
  sdstolower(c->request);

  sdsrange(c->rbuf, pos - c->rbuf + 2, -1);

  return c->request;
}

bool qn_client_read(struct qn_client *c)
{
  c->rbuf = sdsMakeRoomFor(c->rbuf, DEFAULT_BUFFER_SIZE);
  ssize_t n = recv(c->fd, c->rbuf + strlen(c->rbuf), DEFAULT_BUFFER_SIZE, 0);
  if (n > 0) {
    sdsIncrLen(c->rbuf, n);
  } else if (n < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
    perror("recv error");
    return false;
  } else if (n == 0 && strlen(c->wbuf) == 0) {
    /* Client socket was closed and there is no pending to write */
    return false;
  }
  return true;
}

bool qn_client_write(struct qn_client *c)
{
  ssize_t n;
  while ((n = send(c->fd, c->wbuf, strlen(c->wbuf), 0)) > 0) {
    sdsrange(c->wbuf, n, -1);
  }
  if (n < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
    perror("send error");
    return false;
  }
  return true;
}

void qn_client_delete(struct qn_client *c)
{
  c->srv->active_clients--;
  c->srv = NULL;
  ev_io_stop(c->loop, &c->read_watcher);
  ev_io_stop(c->loop, &c->write_watcher);
  sdsfree(c->request);
  sdsfree(c->wbuf);
  sdsfree(c->rbuf);
  close(c->fd);
  HASH_DEL(server.clients, c);
  free(c);
}

void qn_client_delete_all()
{
  struct qn_client *current, *tmp;

  HASH_ITER(hh, server.clients, current, tmp) {
    qn_client_delete(current);
  }
}

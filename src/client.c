#include "qnd.h"

extern struct qn_server server;

void qn_client_add(int fd)
{
  struct qn_client *c;

  HASH_FIND_INT(server.clients, &fd, c);
  if (c == NULL) {
    c = (struct qn_client *)malloc(sizeof(struct qn_client));
    c->fd = fd;
    c->rbuf = sdsempty();
    c->wbuf = sdsempty();
    c->request = sdsempty();
    c->response = sdsempty();
    c->srv = &server;
    HASH_ADD_INT(server.clients, fd, c);
  }
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

  c->request = sdscpylen(c->request, c->rbuf, pos - c->rbuf + 2);
  sdstolower(c->request);

  sdsrange(c->rbuf, pos - c->rbuf + 2, -1);

  return c->request;
}

ssize_t qn_client_read(struct qn_client *c)
{
  size_t oldlen = sdslen(c->rbuf);
  c->rbuf = sdsMakeRoomFor(c->rbuf, DEFAULT_BUFFER_SIZE);
  ssize_t nread = recv(c->fd, c->rbuf+oldlen, DEFAULT_BUFFER_SIZE, 0);
  if (nread > 0) {
    sdsIncrLen(c->rbuf, nread);
  }
  return nread;
}

ssize_t qn_client_write(struct qn_client *c)
{
  ssize_t total = 0;

  while (1) {
    char *pos = strstr(c->wbuf, "\r\n\r\n");
    if (pos == NULL) break;

    c->response = sdscpylen(c->response, c->wbuf, pos - c->wbuf + 4);

    ssize_t bytes = send(c->fd, c->response, sdslen(c->response), 0);

    if (bytes < 0) {
      perror("send error");
      break;
    }

    total += bytes;
    if (bytes < sdslen(c->response)) break;

    sdsrange(c->wbuf, bytes, -1);
    sdsclear(c->response);
  }

  return total;
}

void qn_client_delete(struct qn_client *c)
{
  c->srv = NULL;
  sdsfree(c->response);
  sdsfree(c->request);
  sdsfree(c->wbuf);
  sdsfree(c->rbuf);
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
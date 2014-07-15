#include "qnd.h"

int qn_server_init(struct qn_server *svr)
{
  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);

  sg_init(1);
  if (sg_drop_privileges() != SG_ERROR_NONE)
    exit(1);

  return 0;
}

int qn_server_listen(struct qn_server *svr, int port)
{
  struct sockaddr_in addr;

  if ((svr->sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    return -1;
  }

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(svr->sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
    perror("bind error");
    return -1;
  }

  if (listen(svr->sd, 2) < 0) {
    perror("listen error");
    return -1;
  }

  return 0;
}

void qn_server_cleanup(struct qn_server *svr)
{
  sg_shutdown();
}

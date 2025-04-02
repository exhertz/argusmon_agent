#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "cpu.h"

#define ERR_SUC    0  // success
#define ERR_REQ    1  // bad request (structure)
#define ERR_PATH   2  // path incorrect or not found
#define ERR_ACCES  3  // permission denied
#define ERR_DAT    4  // error get data

typedef struct {
  char path[64];
  char data[960];
} Request;

typedef struct {
  int status;
  char data[1020];
} Response;


/*
 * based on https://www.rsdn.org/article/unix/sockets.xml
 */

int main() {
  int sock, listener;
  struct sockaddr_in addr;
  char buf[1024];
  int bytes_read;

  printf("server started\n");

  listener = socket(AF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    perror("err socket: ");
    close(listener);
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3444);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int bind_res = bind(listener, (struct sockaddr*)&addr, sizeof(addr));
  if (bind_res < 0) {
    perror("bind: ");
    close(listener);
    exit(1);
  }

  int listen_res = listen(listener, 1);
  if (listen_res < 0) {
    perror("listen: ");
    close(listener);
    exit(1);
  }

  while(1) {
    sock = accept(listener, NULL, NULL);
    if (sock < 0) {
      perror("accept: ");
      exit(1);
    }

    bytes_read = read(sock, buf, sizeof(buf));
    if (bytes_read < 0) {
      perror("read\n");
      close(sock);
      continue;
    }

    buf[bytes_read] = '\0';
    printf("%s\n", buf);

    Request* req = (Request*)buf;
    printf("req path: %s\n", req->path);
    printf("req data: %s\n", req->data);

    Response resp;
    memset(&resp, 0, sizeof(resp));

    if (!strcmp(req->path, "getCpuModel")) {
      resp.status = ERR_SUC;
      char cpu[64] = {0};
      cpu_model(cpu);
      strcpy(resp.data, cpu);
    } else if (!strcmp(req->path, "getCpuStat")) {
      uint64_t total = 0;
      uint64_t idle = 0;
      if (cpu_stat(&total, &idle) != 0) {
        resp.status = ERR_DAT;
      } else {
        resp.status = ERR_SUC;
	snprintf(resp.data, sizeof(resp.data), "%lu %lu", total, idle);
      }

    } else {
      resp.status = ERR_PATH;
      printf("error code response: %d\n", resp.status);
    }

    /*

    Response resp;
    memset(&resp, 0, sizeof(resp));
    resp.status = 0;
    strcpy(resp.data, "ok!");
    */

    int send_res = send(sock, &resp, sizeof(resp), 0);
    if (send_res < 0) {
      perror("send failed");
    }

    close(sock);
  }

  close(listener);
  return 0;
}

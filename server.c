#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "cpu.h"
#include "disk.h"
#include "ram.h"
#include "net.h"

#define ERR_SUC    0  // success
#define ERR_REQ    1  // bad request (structure)
#define ERR_PATH   2  // path incorrect or not found
#define ERR_ACCES  3  // permission denied
#define ERR_DAT    4  // error get data

typedef struct {
  int id;
  char path[64];
  char data[956]; // 1024 - 4 - 64
} Request;

typedef struct {
  int request_id;
  int status;
  char data[1016]; // 1024 - 4 - 4
} Response;


/*
 * based on https://www.rsdn.org/article/unix/sockets.xml
 */

int start_server() {
  struct sockaddr_in addr;
  char buf[1024];

  printf("server started\n");

  int listener = socket(AF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    perror("socket initialization error");
    close(listener);
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3444);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int bind_res = bind(listener, (struct sockaddr*)&addr, sizeof(addr));
  if (bind_res < 0) {
    perror("bind listener error");
    close(listener);
    exit(1);
  }

  int listen_res = listen(listener, 1);
  if (listen_res < 0) {
    perror("listen error");
    close(listener);
    exit(1);
  }

  while(1) {
    int sock = accept(listener, NULL, NULL);
    if (sock < 0) {
      perror("accept connection error");
      continue;
    }

    int bytes_read = read(sock, buf, sizeof(buf));
    if (bytes_read < 0) {
      perror("read connection data buffer error");
      close(sock);
      continue;
    }

    buf[bytes_read] = '\0';
    printf("%s\n", buf);

    Request* req = (Request*)buf;
    printf("req id: %d\n", req->id);
    printf("req path: %s\n", req->path);
    printf("req data: %s\n", req->data);

    Response resp = {0};
    resp.request_id = req->id;

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
    } else if (!strcmp(req->path, "getDiskStat")) {
      uint64_t total = 0;
      uint64_t free = 0;
      total = disk_total();
      free = disk_free();

      if (!total || !free) {
        resp.status = ERR_DAT;
      } else {
        resp.status = ERR_SUC;
        snprintf(resp.data, sizeof(resp.data), "%lu %lu", total, free);
      }
    } else if (!strcmp(req->path, "getRamStat")) {
      uint64_t total, usage, available, cached, free;
      int res = ram_stat(&total, &usage, &available, &cached, &free);

      if (res != 0) {
        resp.status = ERR_DAT;
      } else {
        resp.status = ERR_SUC;
        snprintf(resp.data, sizeof(resp.data), "%lu %lu %lu %lu %lu", total, usage, available, cached, free);
      }
    } else if (!strcmp(req->path, "getNetStat")) {
      char iface[64] = {0};
      int res = default_iface(iface);

      if (res != 0) {
        resp.status = ERR_DAT;
      } else {
        uint64_t rx, tx;
        int resstat = net_stat(iface, &rx, &tx);
        if (resstat != 0) {
          resp.status = ERR_DAT;
        } else {
          resp.status = ERR_SUC;
          snprintf(resp.data, sizeof(resp.data), "%lu %lu", rx, tx);
        }
      }
    } else {
      resp.status = ERR_PATH;
      printf("error code response: %d\n", resp.status);
    }

    if (send(sock, &resp, sizeof(resp), 0) < 0) {
      perror("send failed");
    }

    close(sock);
  }

  close(listener);
  return 0;
}

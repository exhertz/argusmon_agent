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

typedef struct __attribute__((packed)) {
  uint32_t id;
  char path[64];
  char data[956]; // 1024 - 4 - 64
} Request;

typedef struct __attribute__((packed)) {
  uint32_t request_id;
  uint32_t status;
  char data[1016]; // 1024 - 4 - 4
} Response;

typedef struct {
  char path[64];
  void (*func)(Request *req, Response *resp);
} Module;

void cpu_model_module(Request *req, Response *resp);
void cpu_stat_module(Request *req, Response *resp);
void disk_module(Request *req, Response *resp);
void ram_module(Request *req, Response *resp);
void net_module(Request *req, Response *resp);

#define MAX_MODULES 5

Module modules[MAX_MODULES] = {
  {"getCpuModel", cpu_model_module},
  {"getCpuStat", cpu_stat_module},
  {"getDiskStat", disk_module},
  {"getRamStat", ram_module},
  {"getNetStat", net_module}
};

void cpu_model_module(Request *req, Response *resp) {
  char cpu[64] = {0};
  cpu_model(cpu);
  if (strlen(cpu) == 0) {
    resp->status = ERR_DAT;
  } else {
    resp->status = ERR_SUC;
    strcpy(resp->data, cpu);
  }
}

void cpu_stat_module(Request *req, Response *resp) {
  uint64_t total = 0;
  uint64_t idle = 0;
  if (cpu_stat(&total, &idle) != 0) {
    resp->status = ERR_DAT;
  } else {
    resp->status = ERR_SUC;
    snprintf(resp->data, sizeof(resp->data), "%lu %lu", total, idle);
  }
}

void disk_module(Request *req, Response *resp) {
  uint64_t total = disk_total();
  uint64_t free = disk_free();

  if (!total || !free) {
    resp->status = ERR_DAT;
  } else {
    resp->status = ERR_SUC;
    snprintf(resp->data, sizeof(resp->data), "%lu %lu", total, free);
  }
}

void ram_module(Request *req, Response *resp) {
  uint64_t total, usage, available, cached, free;
  int res = ram_stat(&total, &usage, &available, &cached, &free);

  if (res != 0) {
    resp->status = ERR_DAT;
  } else {
    resp->status = ERR_SUC;
    snprintf(resp->data, sizeof(resp->data), "%lu %lu %lu %lu %lu", total, usage, available, cached, free);
  }
}

void net_module(Request *req, Response *resp) {
  char iface[64] = {0};
  resp->status = ERR_DAT;

  if (default_iface(iface) == 0) {
    uint64_t rx, tx;
    if (net_stat(iface, &rx, &tx) == 0) {
      resp->status = ERR_SUC;
      snprintf(resp->data, sizeof(resp->data), "%lu %lu", rx, tx);
    }
  }
}

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

    /*
     * while in case of processing multiple requests on one open connection
     */
    while(1) {
      int bytes_read = read(sock, buf, sizeof(buf));
      if (bytes_read <= 0) {
        if (bytes_read == 0) {
          printf("connection closed by client\n");
        } else {
          perror("read connection data buffer error");
        }
        break;
      }

      buf[bytes_read] = '\0';
      printf("%s\n", buf);

      Request *req = (Request*)buf;
      printf("req id: %d\n", req->id);
      printf("req path: %s\n", req->path);
      printf("req data: %s\n", req->data);

      Response resp = {0};
      resp.request_id = req->id;
      resp.status = ERR_PATH;

      for (int i = 0; i < MAX_MODULES; i++) {
        if (strcmp(req->path, modules[i].path) == 0) {
          modules[i].func(req, &resp);
          break;
        }
      }

      if (send(sock, &resp, sizeof(resp), 0) < 0) {
        perror("send failed");
        break;
      }
    }

    close(sock);
  }

  close(listener);
  return 0;
}

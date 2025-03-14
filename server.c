#include <stdio.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

/*
 * based on https://www.rsdn.org/article/unix/sockets.xml
 */

int main() {
  int sock, listener;
  struct sockaddr_in addr;
  char buf[1024];
  int bytes_read;

  listener = socket(AF_INET, SOCK_STREAM, 0);
  if (listener < 0) {
    printf("err socket");
    close(listener);
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3426);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  int bind_res = bind(listener, (struct sockaddr*)&addr, sizeof(addr));
  if (bind_res < 0) {
    printf("bind");
    close(listener);
    exit(1);
  }

  int listen_res = listen(listener, 1);
  if (listen_res < 0) {
    close(listener);
    exit(1);
  }

  while(1) {
    sock = accept(listener, NULL, NULL);
    if (sock < 0) {
      printf("accept");
      exit(1);
    }

    bytes_read = read(sock, buf, sizeof(buf));
    if (bytes_read < 0) {
      printf("read");
      return 1;
    }
    printf("%s\n", buf);

    char* response = "ok!";
    send(sock, response, strlen(response), 0);

    close(sock);
  }

  return 0;
}

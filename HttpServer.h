#pragma once
#include "HttpUtils.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#define SOCKET int
#endif

class HttpServer : public HttpUtils {
  private:
    SOCKET server_fd;
    int port;

    void handle_client(SOCKET client_socket);

  public:
    HttpServer(int port = 6969);
    ~HttpServer();
    void run();
    void initialize_routes() override;
};


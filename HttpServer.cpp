#include "HttpServer.h"
#include <iostream>
#include <thread>
#include <sstream>

#ifdef _WIN32
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

HttpServer::HttpServer(int port) : port(port) {
#ifdef _WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  initialize_mime_types();
  initialize_routes();
}

HttpServer::~HttpServer() {
  closesocket(server_fd);
#ifdef _WIN32
  WSACleanup();
#endif
}

void HttpServer::initialize_routes() {
  get_routes["/"] = [this](const std::string&) { return handle_get_request("/index.html"); };

  post_routes["/api/echo"] = [](const std::string& body) {
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"echo\": \"" + body + "\"}";
  };
  post_routes["/api/reverse"] = [](const std::string& body) {
    std::string reversed(body.rbegin(), body.rend());
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"reversed\": \"" + reversed + "\"}";
  };
}

void HttpServer::handle_client(SOCKET client_socket) {
  char buffer[4096];
  int read_bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
  if (read_bytes <= 0) {
    closesocket(client_socket);
    return;
  }
  buffer[read_bytes] = '\0';

  std::istringstream request_stream(buffer);
  std::string method, path, version;
  request_stream >> method >> path >> version;

  std::string response;
  if (method == "GET") {
    response = handle_get_request(path);
  } else {
    response = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMethod not allowed";
  }

  send(client_socket, response.c_str(), response.size(), 0);
  closesocket(client_socket);
}

void HttpServer::run() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
  listen(server_fd, SOMAXCONN);

  std::cout << "Server listening on port " << port << std::endl;

  while (true) {
    sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);
    SOCKET client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_size);
    std::thread(&HttpServer::handle_client, this, client_socket).detach();
  }
}


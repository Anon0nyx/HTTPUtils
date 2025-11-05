#include "HttpServer.h"
#include <iostream>

int main(int argc, char* argv[]) {
  int port = 6969;
  if (argc > 1) {
    try {
      port = std::stoi(argv[1]);
    } catch (...) {
      std::cerr << "Invalid port. Using default 6969.\n";
    }
  }

  HttpServer server(port);
  server.run();
  return 0;
}


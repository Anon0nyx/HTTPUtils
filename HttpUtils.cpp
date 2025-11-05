#include "HttpUtils.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iostream>

void HttpUtils::initialize_mime_types() {
  mime_types[".html"] = "text/html";
  mime_types[".css"] = "text/css";
  mime_types[".js"] = "application/javascript";
  mime_types[".png"] = "image/png";
  mime_types[".jpg"] = "image/jpeg";
  mime_types[".gif"] = "image/gif";
  mime_types[".svg"] = "image/svg+xml";
  mime_types[".json"] = "application/json";
}

std::string HttpUtils::get_mime_type(const std::string& path) {
  size_t dot_pos = path.find_last_of(".");
  if (dot_pos != std::string::npos) {
    std::string ext = path.substr(dot_pos);
    auto it = mime_types.find(ext);
    if (it != mime_types.end()) return it->second;
  }
  return "application/octet-stream";
}

std::string HttpUtils::get_file_content(const std::string& path) {
#ifdef _WIN32
  std::string full_path = "./templates/" + path;
#else
  std::string full_path = "./templates/" + path;
#endif
  std::ifstream file(full_path, std::ios::binary);
  if (!file) return "";
  return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string HttpUtils::get_current_time() {
  time_t now = time(0);
  struct tm *tstruct = localtime(&now);
  char buf[80];
  strftime(buf, sizeof(buf), "%Y-%m-%d %X", tstruct);
  return buf;
}

std::string HttpUtils::handle_get_request(const std::string& path) {
  std::string file_path = (path == "/") ? "index.html" : path;
  auto route = get_routes.find(file_path);
  if (route != get_routes.end()) return route->second(file_path);

  std::string content = get_file_content(file_path);
  if (content.empty()) return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNot Found";

  std::ostringstream response;
  response << "HTTP/1.1 200 OK\r\n"
    << "Content-Type: " << get_mime_type(file_path) << "\r\n"
    << "Content-Length: " << content.size() << "\r\n\r\n"
    << content;
  return response.str();
}

std::string HttpUtils::handle_post_request(const std::string& path, const std::string& body) {
  auto route = post_routes.find(path);
  if (route != post_routes.end()) return route->second(body);
  return "HTTP/1.1 404 Not Found\r\n\r\nEndpoint not found";
}


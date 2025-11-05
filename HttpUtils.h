#pragma once
#include <string>
#include <unordered_map>
#include <functional>

class HttpUtils {
  protected:
    std::unordered_map<std::string, std::string> mime_types;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> get_routes;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> post_routes;

    void initialize_mime_types();
    std::string get_mime_type(const std::string& path);
    std::string get_file_content(const std::string& path);
    std::string get_current_time();

  public:
    virtual void initialize_routes() = 0;
    virtual std::string handle_get_request(const std::string& path);
    virtual std::string handle_post_request(const std::string& path, const std::string& body);
};


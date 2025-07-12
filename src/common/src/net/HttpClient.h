#pragma once

#include "ClientWithHttp.h"
#include <nlohmann/json.hpp>
#include <nn/os/os_MutexTypes.h>

struct HttpResponse {
    int statusCode;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    // An error occurred in the client itself!
    int errorCode = 0;
    std::string errorMessage;

    bool isError() const {
        return errorCode != 0 || statusCode >= 400;
    }

    nlohmann::json json() {
        if (errorCode != 0) return nlohmann::json();
        return nlohmann::json::parse(body, nullptr, false);
    }

    static HttpResponse of(int statusCode, const std::unordered_map<std::string, std::string>& headers, const std::string& body) {
        HttpResponse response { statusCode, headers, body };
        return response;
    }

    static HttpResponse error(int errCode, const std::string& errorMessage) {
        HttpResponse response {
            .errorCode = errCode,
            .errorMessage = errorMessage,
        };
        return response;
    }
};

class HttpClient : public ClientWithHttp {
    HttpResponse execute(const std::string& url, const std::string& method, const std::unordered_map<std::string, std::string>& headers, const std::string& body = "");

public:
    HttpClient(const std::unordered_map<std::string, std::string>& defaultHeaders);
    ~HttpClient() = default;

    HttpResponse get(const std::string& url, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse post(const std::string& url, const std::string& body, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse post(const std::string& url, const nlohmann::json& body, const std::unordered_map<std::string, std::string>& headers = {});
    HttpResponse post(const std::string& url, const std::initializer_list<std::pair<std::string, std::string>>& formData, const std::unordered_map<std::string, std::string>& headers = {});

    static HttpClient& instance();
    // TODO: support application/form-data?
};

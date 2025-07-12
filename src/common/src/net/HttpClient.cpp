#include "HttpClient.h"
#include <sstream>
#include <hk/svc/api.h>
#include <logger/logger.h>
#include <nn/os.h>
#include <nn/os/os_Mutex.h>

HttpClient::HttpClient(const std::unordered_map<std::string, std::string>& defaultHeaders) {
    this->defaultHeaders = defaultHeaders;
}

HttpResponse HttpClient::execute(const std::string& url, const std::string& method, const std::unordered_map<std::string, std::string>& headers, const std::string& body) {
    auto res = SocketBase::parseURL(url);
    if (!res.has_value() || url.starts_with("ws")) return HttpResponse::error(-1, "Invalid URL");
    auto tuple = res.value();
    auto headersCopy = headers;
    headersCopy.emplace("Host", std::get<1>(tuple));

    auto sock = makeSocket(std::get<0>(tuple));
    sock->connect(std::get<1>(tuple), std::get<2>(tuple));

    HttpResponse response;

    auto packet = buildRequest(method, std::get<3>(tuple), headersCopy, body);
    Logger::log("HTTP Request:\n%s\n", packet.c_str());

    if (auto sent = sock->sendAll(packet); sent < 0) {
        sock->close();
        return HttpResponse::error(-2, "Failed to send request :" + std::to_string(sent));
    }

    std::string responseBody;
    char buffer[1024] = {0};
    auto readTotal = 0;
    do {
        auto read = sock->recv(buffer, sizeof(buffer));
        if (read < 0) {
            break;
        }
        if (read == 0 && !responseBody.empty()) break;
        if (read == 0) {
            nn::os::SleepThread(nn::TimeSpan(1000));
            continue;
        }
        readTotal += read;
        responseBody.append(buffer, read);
    } while (true);

    Logger::log("HTTP Response:\n%s\n", responseBody.c_str());
    auto result = parseResponse(responseBody);

    response.statusCode = std::get<0>(result);
    response.headers = std::get<1>(result);
    response.body = std::get<2>(result);

end:
    sock->close();
    if (response.statusCode >= 300 && response.statusCode < 400) {
        if (auto loc = response.headers.find("Location"); loc != response.headers.end()) {
            return execute(loc->second, method, headers, body);
        }
    }

    return response;
}

HttpResponse HttpClient::get(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return execute(url, "GET", headers);
}

HttpResponse HttpClient::post(const std::string& url, const std::string& body, const std::unordered_map<std::string, std::string>& headers) {
    return execute(url, "POST", headers, body);
}

std::string urlEncode(const char* value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    auto len = strlen(value);

    for (int i = 0; i < len; ++i) {
        auto c = value[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return escaped.str();
}

HttpResponse HttpClient::post(const std::string& url, const std::initializer_list<std::pair<std::string, std::string>>& formData, const std::unordered_map<std::string, std::string>& headers) {
    auto headersCopy = std::unordered_map(headers);
    headersCopy.emplace("Content-Type", "application/x-www-form-urlencoded");

    std::string body;
    for (auto it = formData.begin(); it != formData.end(); ++it) {
        if (it != formData.begin()) body += "&";
        body += urlEncode(it->first.c_str()) + "=" + urlEncode(it->second.c_str());
    }

    return execute(url, "POST", headersCopy, body);
}

HttpResponse HttpClient::post(const std::string& url, const nlohmann::json& body, const std::unordered_map<std::string, std::string>& headers) {
    auto headersCopy = headers;
    headersCopy.emplace("Content-Type", "application/json");

    return execute(url, "POST", headersCopy, body.dump());
}

HttpClient& HttpClient::instance() {
    static HttpClient s_instance({});
    return s_instance;
}

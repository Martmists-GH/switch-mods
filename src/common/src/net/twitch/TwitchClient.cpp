#include "TwitchClient.h"

#include <checks.hpp>
#include <string>
#include <utility>
#include <logger/logger.h>
#include <util/FileUtil.h>
#include <util/MessageUtil.h>

namespace twitch {
    struct TwitchStorage {
        char refreshToken[256];
    };

    void TwitchClient::saveTokens() const {
        TwitchStorage storage = {};
        memcpy(&storage.refreshToken, refreshToken.c_str(), refreshToken.length());
        storage.refreshToken[refreshToken.length()] = '\0';

        FileUtil::writeFile("sd:/mod_data/twitch.tokens", &storage, sizeof(TwitchStorage));
    }
    void TwitchClient::refreshTokens() {
        auto response = client.post("https://id.twitch.tv/oauth2/token", {
                {"grant_type",    "refresh_token"},
                {"client_id",     clientId},
                {"refresh_token", refreshToken},
        });

        if (response.isError()) {
            tokensReady = false;
        } else {
            auto body = response.json();
            accessToken = body["access_token"].get<std::string>();
            refreshToken = body["refresh_token"].get<std::string>();

            tokensReady = true;
            saveTokens();
        }
    }

    TwitchClient::TwitchClient(std::string clientId,
                               std::string scopes) : client(HttpClient({})),
                                                     clientId(std::move(clientId)),
                                                     scopes(std::move(scopes)),
                                                     tokensReady(false) {
        Logger::log("Twitch client created\n");

        if (!FileUtil::exists("sd:/mod_data/twitch.tokens")) {
            FileUtil::createDirectory("sd:/mod_data");
        }
        else {
            auto buffer = FileUtil::readFile("sd:/mod_data/twitch.tokens");
            auto storage = buffer.value().buffer;

            auto storagePtr = static_cast<TwitchStorage*>(storage.get());
            refreshToken = (storagePtr)->refreshToken;

            refreshTokens();
        }
    }

    void TwitchClient::invalidateTokens() {
        tokensReady = false;
    }

    bool TwitchClient::obtainTokens() {
        if (tokensReady) return true;  // Restored from save

        auto data = client.post(
            "https://id.twitch.tv/oauth2/device",
            {
                {"client_id", clientId},
                {"scopes",    scopes}
            }
        );
        if (data.isError()) {
            Logger::log("Error: %d - %s\n", data.errorCode, data.errorMessage.c_str());
            return false;
        }
        auto body = data.json();
        auto uri = body["verification_uri"].get<std::string>();

        if (is_emulator()) {
            // EMULATOR
            MessageUtil::popup(
                0,
                "[DO NOT SHARE] Action Required!",
                "Emulator detected! Unable to open Switch Browser.\n\n"
                "Please open the following URL in your browser:\n" + uri + "\n\n\n"
                "After authorizing the application, you can close this menu\n"
                "DO NOT CLOSE THIS POPUP BEFORE AUTHORIZING!"
            );
        } else {
            MessageUtil::popup(
                0,
                "[DO NOT SHARE] Action Required!",
                "Switch detected!\n\n"
                "Please open the following URL in your browser:\n" + uri + "\n\n\n"
                "After authorizing the application, you can close this menu\n"
                "DO NOT CLOSE THIS POPUP BEFORE AUTHORIZING!"
            );
        }

        data = client.post(
            "https://id.twitch.tv/oauth2/token",
            {{"client_id",   clientId},
             {"scope",       scopes},
             {"grant_type",  "urn:ietf:params:oauth:grant-type:device_code"},
             {"device_code", body["device_code"].get<std::string>()}}
        );
        if (data.isError()) {
            auto bodyStr = data.json().dump().c_str();
            Logger::log("Error: %s\n", bodyStr);
            return false;
        }

        body = data.json();
        accessToken = body["access_token"].get<std::string>();
        refreshToken = body["refresh_token"].get<std::string>();
        saveTokens();

        return true;
    }

    std::optional<UserInfo> TwitchClient::getUserData() {
        auto response = client.get(
            "https://api.twitch.tv/helix/users",
            {{"Authorization", "Bearer " + accessToken},
             {"Client-Id", clientId}}
        );

        static bool isRecursive = false;
        if (response.statusCode == 401 && !isRecursive) {
            refreshTokens();
            isRecursive = true;
            auto res = getUserData();
            isRecursive = false;
            return res;
        }

        if (response.isError()) {
            return std::nullopt;
        }

        return UserInfo(response.json()["data"][0]);
    }

    std::optional<EventsubSubscription> TwitchClient::subscribeEvent(const std::string& event, const std::string& broadcaster, const std::string& session) {
        auto response = client.post(
            "https://api.twitch.tv/helix/eventsub/subscriptions",
            {{"type", event},
             {"version", "1"},
             {"condition",
              {{"broadcaster_user_id", broadcaster}}},
             {"transport",
              {{"method", "websocket"},
               {"session_id", session}}}},
            {{"Authorization", "Bearer " + accessToken},
             {"Client-Id", clientId}}
        );

        static bool isRecursive = false;
        if (response.statusCode == 401 && !isRecursive) {
            refreshTokens();
            isRecursive = true;
            auto res = subscribeEvent(event, broadcaster, session);
            isRecursive = false;
            return res;
        }

        if (response.isError()) {
            return std::nullopt;
        }

        return EventsubSubscription(response.json()["data"][0]);
    }

    std::optional<Poll> TwitchClient::createPoll(const std::string& broadcaster, const std::string& title, const std::vector<std::string>& options, int duration) {
        auto body = nlohmann::json {
            {"broadcaster_id", broadcaster},
            {"title", title},
            {"duration", duration},
            {"choices", nlohmann::json::array()}
        };

        int i = 0;
        for (auto option : options) {
            body["choices"][i++] = {{"title", option}};
        }

        auto response = client.post(
            "https://api.twitch.tv/helix/polls",
            body,
            {{"Authorization", "Bearer " + accessToken},
             {"Client-Id", clientId}}
        );

        static bool isRecursive = false;
        if (response.statusCode == 401 && !isRecursive) {
            refreshTokens();
            isRecursive = true;
            auto res = createPoll(broadcaster, title, options, duration);
            isRecursive = false;
            return res;
        }

        if (response.isError()) {
            return std::nullopt;
        }

        return Poll(response.json()["data"][0]);
    }
}

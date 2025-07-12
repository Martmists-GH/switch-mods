#pragma once
#include <net/HttpClient.h>
#include <net/twitch/HelixTypes.h>

namespace twitch {
    class TwitchClient {
        HttpClient client;
        std::string clientId;
        std::string scopes;
        std::string accessToken;
        std::string refreshToken;
        bool tokensReady;

        void saveTokens() const;
        void refreshTokens();

    public:
        TwitchClient(std::string clientId, std::string scopes);
        void invalidateTokens();
        bool obtainTokens();

        // TODO: Support entire Helix API?
        std::optional<UserInfo> getUserData();
        std::optional<EventsubSubscription> subscribeEvent(const std::string& event, const std::string& broadcaster, const std::string& session);
        std::optional<Poll> createPoll(const std::string& broadcaster, const std::string& title, const std::vector<std::string>& options, int duration);
    };
}

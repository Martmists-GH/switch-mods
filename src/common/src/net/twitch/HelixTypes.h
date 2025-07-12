#pragma once

namespace twitch {
    struct UserInfo {
        std::string id;
        std::string login;
        std::string displayName;
        std::string type;
        std::string broadcasterType;
        std::string description;
        std::string profileImageUrl;
        std::string offlineImageUrl;
        std::optional<std::string> email;  // TODO
        std::string createdAt;

        explicit UserInfo(const nlohmann::json& info) : id(info["id"]), login(info["login"]),
                                                        displayName(info["display_name"]), type(info["type"]),
                                                        broadcasterType(info["broadcaster_type"]),
                                                        description(info["description"]),
                                                        profileImageUrl(info["profile_image_url"]),
                                                        offlineImageUrl(info["offline_image_url"]),
                                                        createdAt(info["created_at"]) {
            if (info.contains("email")) {
                email = info["email"];
            }
        }
    };

    struct EventsubSubscription {
        std::string id;
        std::string status;
        std::string type;
        std::string version;
        nlohmann::json condition;  // TODO
        std::string createdAt;
        nlohmann::json transport;  // TODO
        int cost;

        explicit EventsubSubscription(const nlohmann::json& info) : id(info["id"]), status(info["status"]),
                                                                    type(info["type"]), version(info["version"]),
                                                                    condition(info["condition"]),
                                                                    createdAt(info["created_at"]),
                                                                    transport(info["transport"]), cost(info["cost"]) {}
    };

    struct Poll {
        std::string id;
        std::string broadcasterId;
        std::string broadcasterName;
        std::string broadcasterLogin;
        std::string title;
        nlohmann::json choices;  // TODO
        bool channelPointsVotingEnabled;
        int channelPointsPerVote;
        std::string status;
        int duration;
        std::string startedAt;
        std::optional<std::string> endedAt;

        explicit Poll(const nlohmann::json& info) : id(info["id"]), broadcasterId(info["broadcaster_id"]),
                                                    broadcasterName(info["broadcaster_name"]),
                                                    broadcasterLogin(info["broadcaster_login"]),
                                                    title(info["title"]), choices(info["choices"]),
                                                    channelPointsVotingEnabled(info["channel_points_voting_enabled"]),
                                                    channelPointsPerVote(info["channel_points_per_vote"]),
                                                    status(info["status"]), duration(info["duration"]),
                                                    startedAt(info["started_at"]) {
            if (info.contains("ended_at")) {
                endedAt = info["ended_at"];
            }
        }
    };
}

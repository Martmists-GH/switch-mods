#pragma once
#include <nlohmann/json.hpp>

namespace ap {
    struct NetworkVersion {
        int major;
        int minor;
        int build;

        NetworkVersion(int major, int minor, int build) : major(major), minor(minor), build(build) {}
        explicit NetworkVersion(const nlohmann::json& json): major(json["major"]), minor(json["minor"]),
                                                             build(json["build"]) {}

        nlohmann::json json() const {
            return {
                {"major", major},
                {"minor", minor},
                {"build", build}
            };
        }
    };
    enum class Permission : int8_t {
        DISABLED = 0,
        ENABLED = 1,
        GOAL = 2,
        AUTO = 6,
        AUTO_ENABLED = 7
    };
    struct Permissions {
        Permission release;
        Permission collect;
        Permission remaining;

        explicit Permissions(const nlohmann::json& json) : release(json["release"]), collect(json["collect"]),
                                                           remaining(json["remaining"]) {}
    };
    struct RoomInfo {
        NetworkVersion version;
        NetworkVersion generatorVersion;
        std::vector<std::string> tags;
        bool password;
        Permissions permissions;
        int hintCost;
        int locationCheckPoints;
        std::vector<std::string> games;
        std::unordered_map<std::string, std::string> datapackageChecksums;
        std::string seedName;
        float time;

        explicit RoomInfo(const nlohmann::json& json) : version(json["version"]),
                                                        generatorVersion(json["generator_version"]), tags(json["tags"]),
                                                        password(json["password"]), permissions(json["permissions"]),
                                                        hintCost(json["hint_cost"]),
                                                        locationCheckPoints(json["location_check_points"]),
                                                        games(json["games"]),
                                                        datapackageChecksums(json["datapackage_checksums"]),
                                                        seedName(json["seed_name"]), time(json["time"]) {}
    };
    struct ConnectionRefused {
        std::vector<std::string> errors;

        explicit ConnectionRefused(const nlohmann::json& json) {
            if (json.contains("errors")) {
                errors = json["errors"];
            }
        }
    };
    struct NetworkPlayer {
        int team;
        int slot;
        std::string alias;
        std::string name;

        explicit NetworkPlayer(const nlohmann::json& json): team(json["team"]), slot(json["slot"]),
                                                            alias(json["alias"]), name(json["name"]) {}
    };
    enum class SlotType : uint8_t {
        SPECTATOR = 0,
        PLAYER = 1,
        GROUP = 2
    };
    struct NetworkSlot {
        std::string name;
        std::string game;
        SlotType type;
        std::vector<int> groupMembers;

        explicit NetworkSlot(const nlohmann::json& json): name(json["name"]), game(json["game"]), type(json["type"]) {
            if (type == SlotType::PLAYER) {
                groupMembers = json["group_members"].get<std::vector<int>>();
            }
        }
    };
    struct Connected {
        int team;
        int slot;
        std::vector<NetworkPlayer> players;
        std::vector<int> missingLocations;
        std::vector<int> checkedLocations;
        nlohmann::json slotData;
        std::unordered_map<std::string, NetworkSlot> slotInfo;
        int hintPoints;

        explicit Connected(const nlohmann::json& json) : team(json["team"]), slot(json["slot"]),
                                                         missingLocations(json["missing_locations"]),
                                                         checkedLocations(json["checked_locations"]),
                                                         hintPoints(json["hint_points"]) {
            for (auto& player : json["players"]) {
                players.emplace_back(player);
            }
            for (auto& entry : json["slot_info"].items()) {
                slotInfo.emplace(entry.key(), entry.value());
            }
            if (json.contains("slot_data")) {
                slotData = json["slot_data"];
            }
        }
    };
    struct NetworkItem {
        int item;
        int location;
        int player;
        int flags;

        explicit NetworkItem(const nlohmann::json& json): item(json["item"]), location(json["location"]),
                                                          player(json["player"]), flags(json["flags"]) {}
    };
    struct ReceivedItems {
        int index;
        std::vector<NetworkItem> items;

        explicit ReceivedItems(const nlohmann::json& json): index(json["index"]) {
            for (auto& item : json["items"]) {
                items.emplace_back(item);
            }
        }
    };
    struct LocationInfo {
        std::vector<NetworkItem> locations;

        explicit LocationInfo(const nlohmann::json& json) {
            for (auto& location : json["locations"]) {
                locations.emplace_back(location);
            }
        }
    };
    struct RoomUpdate {
        // TODO: Implement this
        // Not implemented yet because everything is optional and that's a lot of effort :(
        explicit RoomUpdate(const nlohmann::json& json) {}
    };
    enum class HintStatus : uint8_t {
        UNSPECIFIED = 0,
        NO_PRIORITY = 10,
        AVOID = 20,
        PRIORITY = 30,
        FOUND = 40
    };
    struct JsonMessagePart {
        std::optional<std::string> type;
        std::optional<std::string> text;
        std::optional<std::string> color;
        std::optional<int> flags;
        std::optional<int> player;
        std::optional<HintStatus> hintStatus;

        explicit JsonMessagePart(const nlohmann::json& json) {
            if (json.contains("type")) {
                type = json["type"];
            }
            if (json.contains("text")) {
                text = json["text"];
            }
            if (json.contains("color")) {
                color = json["color"];
            }
            if (json.contains("flags")) {
                flags = json["flags"];
            }
            if (json.contains("player")) {
                player = json["player"];
            }
            if (json.contains("hint_status")) {
                hintStatus = json["hint_status"];
            }
        }
    };
    struct PrintJson {
        std::vector<JsonMessagePart> data;
        std::optional<std::string> type;
        int receiving;
        NetworkItem item;
        bool found;
        int team;
        int slot;
        std::string message;
        std::vector<std::string> tags;
        int countdown;

        explicit PrintJson(const nlohmann::json& json): receiving(json["receiving"]),
                                                        item(json["item"]), found(json["found"]), team(json["team"]),
                                                        slot(json["slot"]), message(json["message"]),
                                                        tags(json["tags"]), countdown(json["countdown"]) {
            for (auto& part : json["data"]) {
                data.emplace_back(part);
            }
            if (json.contains("type")) {
                type = json["type"];
            }
        }
    };
    struct DataPackage {
        // TODO: Proper types
        nlohmann::json data;

        explicit DataPackage(const nlohmann::json& json): data(json["data"]) {}
    };
    struct Bounced {
        std::vector<std::string> games;
        std::vector<int> slots;
        std::vector<std::string> tags;
        nlohmann::json data;

        explicit Bounced(const nlohmann::json& json): data(json["data"]) {
            if (json.contains("games")) {
                games = json["games"];
            }
            if (json.contains("slots")) {
                slots = json["slots"].get<std::vector<int>>();
            }
            if (json.contains("tags")) {
                tags = json["tags"];
            }
        }
    };
    struct InvalidPacket {
        std::string type;
        std::optional<std::string> originalCmd;
        std::string text;

        explicit InvalidPacket(const nlohmann::json& json): type(json["type"]), text(json["text"]) {
            if (json.contains("original_cmd")) {
                originalCmd = json["original_cmd"];
            }
        }
    };
    struct Retrieved {
        nlohmann::json keys;

        explicit Retrieved(const nlohmann::json& json): keys(json["keys"]) {}
    };
    struct SetReply {
        std::string key;
        nlohmann::json value;
        nlohmann::json originalValue;
        int slot;

        explicit SetReply(const nlohmann::json& json): key(json["key"]), value(json["value"]),
                                                       originalValue(json["original_value"]), slot(json["slot"]) {}
    };
    struct Connect {
        std::optional<std::string> password;
        std::string game;
        std::string name;
        std::string uuid;
        NetworkVersion version;
        int itemsHandling;
        std::vector<std::string> tags;
        bool slotData;

        nlohmann::json json() const {
            nlohmann::json payload = {
                {"cmd", "Connect"},
                {"game", game},
                {"name", name},
                {"uuid", uuid},
                {"version", version.json()},
                {"items_handling", itemsHandling},
                {"tags", tags},
                {"slot_data", slotData}
            };
            if (password.has_value()) {
                payload["password"] = password.value();
            }
            return payload;
        }
    };
    struct ConnectUpdate {
        int itemsHandling;
        std::vector<std::string> tags;

        nlohmann::json json() const {
            return {
                {"cmd", "ConnectUpdate"},
                {"items_handling", itemsHandling},
                {"tags", tags},
            };
        }
    };
    struct Sync {
        nlohmann::json json() const {
            return {
                {"cmd", "Sync"},
            };
        }
    };
    struct LocationChecks {
        std::vector<int> locations;

        nlohmann::json json() const {
            return {
                {"cmd", "LocationChecks"},
                {"locations", locations},
            };
        }
    };
    struct LocationScouts {
        std::vector<int> locations;
        int createAsHint;

        nlohmann::json json() const {
            return {
                {"cmd", "LocationChecks"},
                {"locations", locations},
                {"create_as_hint", createAsHint}
            };
        }
    };
    struct UpdateHint {
        int player;
        int location;
        std::optional<HintStatus> status;

        nlohmann::json json() const {
            nlohmann::json payload = {
                {"cmd", "UpdateHint"},
                {"player", player},
                {"location", location},
            };
            if (status.has_value()) {
                payload["status"] = status.value();
            }
            return payload;
        }
    };
    enum class ClientStatus : uint8_t {
        UNKNOWN = 0,
        CONNECTED = 5,
        READY = 10,
        PLAYING = 20,
        GOAL = 30
    };
    struct StatusUpdate {
        ClientStatus status;

        nlohmann::json json() const {
            return {
                {"cmd", "StatusUpdate"},
                {"status", status},
            };
        }
    };
    struct Say {
        std::string text;

        nlohmann::json json() const {
            return {
                {"cmd", "StatusUpdate"},
                {"text", text},
            };
        }
    };
    struct GetDataPackage {
        std::vector<std::string> games;

        nlohmann::json json() const {
            return {
                {"cmd", "GetDataPackage"},
                {"games", games},
            };
        }
    };
    struct Bounce {
        std::vector<std::string> games;
        std::vector<int> slots;
        std::vector<std::string> tags;
        nlohmann::json data;

        nlohmann::json json() const {
            return {
                {"cmd", "Bounce"},
                {"games", games},
                {"slots", slots},
                {"tags", tags},
                {"data", data},
            };
        }
    };
    struct Get {
        std::vector<std::string> keys;

        nlohmann::json json() const {
            return {
                {"cmd", "Get"},
                {"keys", keys},
            };
        }
    };
    struct Set {
        std::string key;
        nlohmann::json defaultValue;
        bool wantReply;
        std::vector<nlohmann::json> operations;

        nlohmann::json json() const {
            return {
                {"cmd", "Set"},
                {"default", defaultValue},
                {"want_reply", wantReply},
                {"operations", operations},
            };
        }
    };
    struct SetNotify {
        std::vector<std::string> keys;

        nlohmann::json json() const {
            return {
                {"cmd", "SetNotify"},
                {"keys", keys},
            };
        }
    };
}

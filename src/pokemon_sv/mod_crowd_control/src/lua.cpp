#include <cc_hooks.h>
#include <random>
#include <hk/types.h>
#include <hk/ro/RoUtil.h>
#include <logger/logger.h>
#include <net/twitch/TwitchClient.h>
#include <net/WebsocketClient.h>
#include <nn/os.h>
#include <lua_utils.h>
#include <util/MessageUtil.h>
#include <util/ThreadUtil.h>

twitch::TwitchClient& getTwitchClient() {
    static twitch::TwitchClient client("hchddcf841qrxjz5ukiixp0m3wc7k4", "channel:manage:polls");
    return client;
}

static bool running = false;
void wsLoop(void* arg) {
    auto twitch = getTwitchClient();

    WebsocketClient ws("wss://eventsub.wss.twitch.tv/ws");
    if (!ws.open()) {
        Logger::log("Failed to set up WS\n");
        return;
    }

    std::mt19937 generator(nn::os::GetSystemTick().value);
    while (running) {
        auto data = ws.receiveJson();
        if (data.empty()) {
            if (!ws.open()) {
                Logger::log("WebSocket Closed!\n");
                break;
            }
            continue;
        };

        auto type = data["metadata"]["message_type"].get<std::string>();

        if (type == "session_welcome") {
            auto session_id = data["payload"]["session"]["id"].get<std::string>();
            auto userData = twitch.getUserData();
            if (!userData.has_value()) {
                Logger::log("Failed to fetch user data!\n");
                break;
            }
            auto subInfo = twitch.subscribeEvent("channel.poll.end", userData.value().id, session_id);
            if (!subInfo.has_value()) {
                Logger::log("Failed to subscribe!\n");
                break;
            }
        } else if (type == "session_reconnect") {
            ws.reconnect(data["payload"]["session"]["reconnect_url"].get<std::string>());
        } else if (type == "notification") {
            auto subscription = data["metadata"]["subscription_type"].get<std::string>();
            if (subscription == "channel.poll.end") {
                if (data["payload"]["event"]["status"].get<std::string>() == "completed") {
                    continue;
                }

                auto choices = data["payload"]["event"]["choices"];

                int maxValue = -1;
                std::vector<std::string> max = {};

                for (auto item: choices) {
                    auto num = item["votes"].get<int>();
                    if (num > maxValue) {
                        max.clear();
                        max.push_back(item["title"].get<std::string>());
                        maxValue = num;
                    } else if (num == maxValue) {
                        max.push_back(item["title"].get<std::string>());
                    }
                }

                std::string chosen;
                if (max.size() > 1) {
                    // select a random one
                    std::uniform_int_distribution<> distribution(0, max.size() - 1);
                    chosen = max[distribution(generator)];
                } else {
                    chosen = max[0];
                }

                Logger::log("Launching event: %s\n", chosen.c_str());

                triggerEvent(chosen);
            } else {
                Logger::log("Unhandled notification: %s\n", subscription.c_str());
            }
        } else if (type == "session_keepalive") {
            // Don't log
            continue;
        } else {
            auto dumped = data.dump();
            Logger::log("Unhandled event: %s\n", dumped.c_str());
        }
    }

    running = false;
}

void eventLoop(void* arg) {
    auto cfg = getConfig();
    auto twitch = getTwitchClient();
    auto data = twitch.getUserData();
    std::string lastEvent = "";

    if (!data.has_value()) {
        Logger::log("Failed to get user info!\n");
        running = false;
        return;
    }

    std::mt19937 generator(nn::os::GetSystemTick().value);
    while (running) {
        auto lang = CC_lang_current();
        auto& strings = getStrings(lang);

        Logger::log("Waiting %d seconds...\n", cfg.pollInterval);

        nn::os::SleepThread(nn::TimeSpan::FromSeconds(cfg.pollInterval + 5)); // add 5 seconds just in case some slowdown happens
        auto allEvents = getEventList();
        auto chosen = std::vector<std::string>();
        std::ranges::shuffle(allEvents, generator);

        Logger::log("Selecting random events\n");
        auto offset = 0;
        for (int i = 0; i < cfg.pollOptions; i++) {
            auto item = allEvents[i + offset];
            if (item == lastEvent) {
                offset += 1;
                chosen.push_back(allEvents[i + offset]);
            } else {
                chosen.push_back(item);
            }
        }

        Logger::log("Creating poll!\n");
        auto res = twitch.createPoll(data.value().id, strings.poll.title, chosen, cfg.pollDuration);
        if (!res.has_value()) {
            Logger::log("Failed to create poll!\n");
            break;
        }

        // char buffer[256] = {};
        auto code = std::format(R"(API:pushNotice("{}"))", strings.poll.announce);
        // sprintf(buffer, "API:pushNotice(\"%s\")", strings.poll.announce);

        executeLua(get_lua_state(), code.c_str(), "<CC:EventLoop:NewPoll>");
    }

    running = false;
    executeLua(get_lua_state(), "API:pushNotice('The mod crashed, please restart!')", "<CC:EventLoop:NewPoll>");
}


bool didInit = false;
static int luaCCStart(lua_State* L) {
    if (didInit) {
        return 0;
    }
    didInit = true;
    running = true;

    Logger::log("Starting Crowd Control!\n");

    static auto wsThread = ThreadUtil::createThread(wsLoop, "CC WS Thread");
    static auto eventThread = ThreadUtil::createThread(eventLoop, "CC Event Thread");

    wsThread->start();
    eventThread->start();
    Logger::log("Crowd Control threads started!\n");

    return 0;
}

static int luaCCInit(lua_State* L) {
    Logger::log("Crowd Control initialization!\n");
    auto twitch = getTwitchClient();
    if (twitch.obtainTokens()) {
        Logger::log("Tokens restored from file!\n");
        if (!twitch.getUserData().has_value()) {
            twitch.invalidateTokens();
            twitch.obtainTokens();
        }
    } else {
        MessageUtil::abort(2, "An error occurred!", "Failed to authenticate with Twitch, please restart the game.\nIf this keeps happening, report the issue to the developer.");
    }

    return 0;
}

static int luaCCTick(lua_State* L) {
    lua_pushinteger(L, nn::os::GetSystemTick().value / (1000 * 1000 * 1000));
    return 1;
}

void lua_mod_code(lua_State* L) {
    lua_pushglobaltable(L);

    lua_pushcfunction(L, luaCCStart);
    lua_setfield(L, -2, "cc_Start");

    lua_pushcfunction(L, luaCCInit);
    lua_setfield(L, -2, "cc_Init");

    lua_pushcfunction(L, luaCCTick);
    lua_setfield(L, -2, "cc_Tick");

    lua_pop(L, 1);
}

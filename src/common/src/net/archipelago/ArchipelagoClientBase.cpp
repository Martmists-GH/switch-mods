#include "ArchipelagoClientBase.h"

#include <logger/logger.h>

namespace ap {
    ArchipelagoClientBase::ArchipelagoClientBase(const std::string& url): client(url) {

    }

    ArchipelagoClientBase::~ArchipelagoClientBase() {
        client.close();
    }

    void ArchipelagoClientBase::sendPacket(const nlohmann::json& packet) {
        if (!client.sendJson(packet)) {
            if (!isDisconnected && !client.open()) {
                isDisconnected = true;
                onDisconnect();
            }
        }
    }

    void ArchipelagoClientBase::handleOnePacket() {
        auto pkt = client.receiveJson();
        if (pkt.empty()) {
            if (!isDisconnected && !client.open()) {
                isDisconnected = true;
                onDisconnect();
            }
            return;
        }

        std::string cmd = pkt["cmd"];

        if (cmd == "RoomInfo") {
            onRoomInfo(RoomInfo(pkt));
        } else if (cmd == "ConnectionRefused") {
            onConnectionRefused(ConnectionRefused(pkt));
        } else if (cmd == "Connected") {
            onConnected(Connected(pkt));
        } else if (cmd == "ReceivedItems") {
            onReceivedItems(ReceivedItems(pkt));
        } else if (cmd == "LocationInfo") {
            onLocationInfo(LocationInfo(pkt));
        } else if (cmd == "RoomUpdate") {
            onRoomUpdate(RoomUpdate(pkt));
        } else if (cmd == "PrintJson") {
            onPrintJson(PrintJson(pkt));
        } else if (cmd == "DataPackage") {
            onDataPackage(DataPackage(pkt));
        } else if (cmd == "Bounced") {
            onBounced(Bounced(pkt));
        } else if (cmd == "InvalidPacket") {
            onInvalidPacket(InvalidPacket(pkt));
        } else if (cmd == "Retrieved") {
            onRetrieved(Retrieved(pkt));
        } else if (cmd == "SetReply") {
            onSetReply(SetReply(pkt));
        } else {
            Logger::log("AP received unknown command: %s\n", cmd.c_str());
        }
    }

    void ArchipelagoClientBase::reconnect(const std::string& url) {
        client.reconnect(url);
        isDisconnected = false;
    }

    void ArchipelagoClientBase::sendConnect(const Connect& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendConnectUpdate(const ConnectUpdate& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendSync(const Sync& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendLocationChecks(const LocationChecks& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendLocationScouts(const LocationScouts& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendUpdateHint(const UpdateHint& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendStatusUpdate(const StatusUpdate& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendSay(const Say& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendGetDataPackage(const GetDataPackage& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendBounce(const Bounce& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendGet(const Get& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendSet(const Set& packet) {
        sendPacket(packet.json());
    }
    void ArchipelagoClientBase::sendSetNotify(const SetNotify& packet) {
        sendPacket(packet.json());
    }
}

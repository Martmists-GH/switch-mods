#pragma once
#include "net/WebsocketClient.h"
#include "net/ap/PacketTypes.h"

namespace ap {

    class ArchipelagoClientBase {
        WebsocketClient client;

    protected:
        virtual void onRoomInfo(const RoomInfo& packet) {};
        virtual void onConnectionRefused(const ConnectionRefused& packet) {};
        virtual void onConnected(const Connected& packet) {};
        virtual void onReceivedItems(const ReceivedItems& packet) {};
        virtual void onLocationInfo(const LocationInfo& packet) {};
        virtual void onRoomUpdate(const RoomUpdate& packet) {};
        virtual void onPrintJson(const PrintJson& packet) {};
        virtual void onDataPackage(const DataPackage& packet) {};
        virtual void onBounced(const Bounced& packet) {};
        virtual void onInvalidPacket(const InvalidPacket& packet) {};
        virtual void onRetrieved(const Retrieved& packet) {};
        virtual void onSetReply(const SetReply& packet) {};

	    // TODO: Easy to use methods that invoke these
        void sendConnect(const Connect& packet);
        void sendConnectUpdate(const ConnectUpdate& packet);
        void sendSync(const Sync& packet);
        void sendLocationChecks(const LocationChecks& packet);
        void sendLocationScouts(const LocationScouts& packet);
        void sendUpdateHint(const UpdateHint& packet);
        void sendStatusUpdate(const StatusUpdate& packet);
        void sendSay(const Say& packet);
        void sendGetDataPackage(const GetDataPackage& packet);
        void sendBounce(const Bounce& packet);
        void sendGet(const Get& packet);
        void sendSet(const Set& packet);
        void sendSetNotify(const SetNotify& packet);
    public:
        ArchipelagoClientBase(const std::string& url);
        virtual ~ArchipelagoClientBase();
    };

}

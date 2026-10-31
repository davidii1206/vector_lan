#pragma once

#include "NetworkWrapper.h"
#include <cstdint>

enum class PacketType : uint8_t  {
    PLAYER_SPAWN = 1,
    JOIN_PACKET = 2,
    JOIN_SUCCESS = 3,
    LOBBY_SNAPSHOT = 4,
    READY_PACKET = 5,
    STATE = 6,
};

struct JoinPacket {
    PacketType type = PacketType::JOIN_PACKET;
    char username[128];
};

struct JoinSuccess {
    PacketType type = PacketType::JOIN_SUCCESS;
    uint32_t client_id;
};

struct LobbySnapshot {
    PacketType type = PacketType::LOBBY_SNAPSHOT;
    uint32_t player_count;
    uint32_t ids[32];
    char usernames[32][128];
    bool is_ready[32];
};

struct ReadyPacket {
    PacketType type = PacketType::READY_PACKET;
    uint32_t id;
    bool ready;
};

struct StatePacket {
    PacketType type = PacketType::STATE;
    uint32_t state;
};
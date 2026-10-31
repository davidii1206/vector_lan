#pragma once

#include "Networking/NetworkWrapper.h"
#include "Networking/Packets.h"
#include "player.h"
#include "context.h"

#include <cstdint>
#include <string>
#include <unordered_map>

class Server {
    public: 
        Server() = default;
        ~Server() = default;

        void add_connection(std::string username, uint32_t id);
        void remove_connection(uint32_t id);

        std::unordered_map<uint32_t, std::string> get_connections();

    private:
        int player_count = 0;
        std::unordered_map<uint32_t, std::string> connections;
};
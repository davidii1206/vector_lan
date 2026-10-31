#include "server.h"
#include <utility>

void Server::add_connection(std::string username, uint32_t id) {
    connections[id] = std::move(username);
    ++player_count;
}

void Server::remove_connection(uint32_t id) {
    if (connections.erase(id) > 0) {
        --player_count;
    }
}

std::unordered_map<uint32_t, std::string> Server::get_connections() {
    return connections;
}
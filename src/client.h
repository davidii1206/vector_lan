#pragma once

#include <cstdint>
#include <string>

class Client {
    public:
        Client(int user_id, std::string name);
        ~Client() = default;

        void toggle_ready();
        bool get_ready();

    private:
        uint32_t id;
        std::string username;
        bool ready;
};
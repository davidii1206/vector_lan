#include "client.h"

Client::Client(int user_id, std::string name) : id(user_id), username(name) {
    ready = false;
}

void Client::toggle_ready() {
    if (ready) {
        ready = false;
    } else {
        ready = true;
    }
}

bool Client::get_ready() {
    return ready;
}
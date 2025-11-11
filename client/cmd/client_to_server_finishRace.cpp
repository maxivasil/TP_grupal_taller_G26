#include "client_to_server_finishRace.h"
#include <iostream>

ClientToServerFinishRace::ClientToServerFinishRace() {}

std::vector<uint8_t> ClientToServerFinishRace::to_bytes() const {
    std::vector<uint8_t> data;
    uint8_t command = CLIENT_TO_SERVER_FINISH_RACE;
    data.push_back(command);
    std::cout << "[CLIENT] Sending finish race notification (command: " 
              << static_cast<int>(command) << ")" << std::endl;
    return data;
}

#include "client_to_server_tour.h"

#include <iostream>
#include <stdexcept>

#include "../../common/constants.h"

#include <yaml-cpp/yaml.h>

#include "../game_logic/Race.h"

ClientToServerTour::ClientToServerTour(std::string& tourFile):
        tourFile(tourFile), ClientToServerCmd_Server(client_id) {}

void ClientToServerTour::execute(ServerContext& ctx) {
    std::cout << "Ejecutando comando Tour con archivo: " << tourFile
              << " del cliente con id: " << client_id << std::endl;

    if (!ctx.inLobby || !*(ctx.inLobby) || !ctx.racesInfo)
        return;

    YAML::Node root = YAML::LoadFile(ABS_DIR + tourFile);

    for (const auto& node: root["races"]) {
        std::string cityStr = node["city"].as<std::string>();
        std::string track = node["track"].as<std::string>();

        CityName city;
        if (cityStr == "Liberty_City") {
            city = CityName::LibertyCity;
        } else if (cityStr == "San_Andreas") {
            city = CityName::SanAndreas;
        } else if (cityStr == "Vice_City") {
            city = CityName::ViceCity;
        } else {
            throw std::runtime_error("TourCmd: ciudad inválida en tour: " + cityStr);
        }

        ctx.racesInfo->emplace_back(city, track);
    }

    for (const auto& raceInfo: *(ctx.racesInfo)) {
        std::cout << "  - Ciudad: " << raceInfo.city << ", Pista: " << raceInfo.trackFile
                  << std::endl;
    }
}

// Deserialización desde bytes
ClientToServerTour* ClientToServerTour::from_bytes(const std::vector<uint8_t>& data,
                                                   const int client_id) {
    if (data.size() < 1) {
        throw std::runtime_error("TourCmd: datos insuficientes");
    }

    std::string tour(data.begin() + 1, data.end());  // data[0] sería el TOUR_COMMAND

    return new ClientToServerTour(tour);
}

#include "client_to_server_tour_server.h"

#include <iostream>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

#include "../../common/constants.h"
#include "../game_logic/Race.h"

ClientToServerTour_Server::ClientToServerTour_Server(std::string tourFile, uint32_t client_id):
        tourFile(tourFile), ClientToServerCmd_Server(client_id) {}

void ClientToServerTour_Server::execute(ServerContext& ctx) {
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
}

// Deserialización desde bytes
ClientToServerTour_Server* ClientToServerTour_Server::from_bytes(const std::vector<uint8_t>& data,
                                                                 const uint32_t client_id) {
    if (data.size() < 1) {
        throw std::runtime_error("TourCmd: datos insuficientes");
    }

    std::string tour(data.begin() + 1, data.end());  // data[0] sería el TOUR_COMMAND

    return new ClientToServerTour_Server(tour, client_id);
}

const std::string& ClientToServerTour_Server::get_tour_file() const { return tourFile; }

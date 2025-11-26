#include "client_handler.h"

#include <string>
#include <utility>

#include "../common/constants.h"

#include "LobbiesMonitor.h"
#include "client_handler.h"

ServerClientHandler::ServerClientHandler(uint32_t client_id, Socket&& s,
                                         LobbiesMonitor& lobbiesMonitor):
        client_id(client_id),
        protocol(std::move(s)),
        send_queue(UINT32_MAX),
        registered_commands(),
        receiver(nullptr),
        sender(protocol, send_queue),
        lobbiesMonitor(lobbiesMonitor) {}

void ServerClientHandler::run() {
    try {
        sender.start();
        bool inLobby = false;
        auto registry = registered_commands.get_recv_registry();
        ServerContext ctx = {.race = nullptr,
                             .client = this,
                             .inLobby = &inLobby,
                             .clientsReady = nullptr,
                             .lobby = nullptr,
                             .racesInfo = nullptr};
        // cppcheck-suppress knownConditionTrueFalse
        while (should_keep_running() && !inLobby) {
            std::vector<uint8_t> data = protocol.recv_full_message();
            if (data.empty())
                continue;
            std::unique_ptr<ClientToServerCmd_Server> cmd(
                    ClientToServerCmd_Server::from_bytes(data, registry, client_id));
            cmd->execute(ctx);
        }

        // cppcheck-suppress knownConditionTrueFalse
        if (!inLobby) {
            return;
        }
        std::cout << "[SERVER] Assigned client ID " << client_id << " to connected client.\n";
        auto assignIdCmd = std::make_shared<ServerToClientAssignId_Server>(client_id);
        send_message(assignIdCmd);

        receiver->start();

        receiver->join();
        sender.join();
        protocol.close_connection();
    } catch (const std::exception& e) {
        if (!should_keep_running()) {
            stop();
            return;
        }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl;
    }
}

void ServerClientHandler::stop() {
    Thread::stop();
    if (receiver && receiver->is_alive()) {
        receiver->stop();
        receiver->join();
    }
    if (!protocol.is_connection_closed()) {
        protocol.close_connection();
    }
    send_queue.close();
    sender.stop();
    sender.join();
}

bool ServerClientHandler::is_dead() const {
    return !should_keep_running() || protocol.is_connection_closed();
}

void ServerClientHandler::send_message(std::shared_ptr<ServerToClientCmd_Server> cmd) {
    if (cmd)
        send_queue.push(cmd);
}

Queue<ClientToServerCmd_Server*>* ServerClientHandler::createLobby(const std::string& lobbyId) {
    return lobbiesMonitor.createLobby(lobbyId, this);
}

Queue<ClientToServerCmd_Server*>* ServerClientHandler::joinLobby(const std::string& lobbyId) {
    return lobbiesMonitor.joinLobby(lobbyId, this);
}

void ServerClientHandler::initReceiver(Queue<ClientToServerCmd_Server*>& gameloop_queue) {
    receiver = std::make_unique<ThreadReceiver>(client_id, protocol, gameloop_queue,
                                                registered_commands.get_recv_registry());
}

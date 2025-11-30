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
                break;
            std::unique_ptr<ClientToServerCmd_Server> cmd(
                    ClientToServerCmd_Server::from_bytes(data, registry, client_id));
            cmd->execute(ctx);
        }

        // cppcheck-suppress knownConditionTrueFalse
        if (!inLobby) {
            return;
        }
        auto assignIdCmd = std::make_shared<ServerToClientAssignId_Server>(client_id);
        send_message(assignIdCmd);

        receiver->start();
    } catch (const std::exception& e) {
        if (!should_keep_running()) {
            return;
        }
        std::cerr << "Unexpected exception in ClientHandler" << e.what() << std::endl;
    }
}

void ServerClientHandler::stop() {
    resetSelfPtr();
    Thread::stop();
    send_queue.close();
    if (!protocol.is_connection_closed()) {
        protocol.close_connection();
    }
    if (receiver) {
        receiver->stop();
        receiver->join();
    }
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
    return lobbiesMonitor.createLobby(lobbyId, self_ptr);
}

Queue<ClientToServerCmd_Server*>* ServerClientHandler::joinLobby(const std::string& lobbyId) {
    return lobbiesMonitor.joinLobby(lobbyId, self_ptr);
}

void ServerClientHandler::initReceiver(Queue<ClientToServerCmd_Server*>& gameloop_queue) {
    receiver = std::make_unique<ThreadReceiver>(client_id, protocol, gameloop_queue,
                                                registered_commands.get_recv_registry());
}

void ServerClientHandler::setSelfPtr(std::shared_ptr<ServerClientHandler> self) { self_ptr = self; }

void ServerClientHandler::resetSelfPtr() { self_ptr.reset(); }

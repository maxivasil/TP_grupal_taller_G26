#include "protected_clients.h"

#include "client_handler.h"

ServerProtectedClients::ServerProtectedClients(): clients() {}


void ServerProtectedClients::broadcast(std::shared_ptr<ServerToClientCmd_Server> cmd) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client: clients) {
        try {
            client->send_message(cmd);
        } catch (const ClosedQueue&) {}
    }
}

void ServerProtectedClients::add_client(std::shared_ptr<ServerClientHandler> client) {
    std::lock_guard<std::mutex> lock(m);
    clients.push_back(client);
}

void ServerProtectedClients::stop_all_and_delete() {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client: clients) {
        if (client == nullptr)
            continue;
        client->stop();
        client->join();
    }
    clients.clear();
}

void ServerProtectedClients::stop_and_delete_dead() {
    std::vector<std::shared_ptr<ServerClientHandler>> to_remove;
    {
        std::lock_guard<std::mutex> lock(m);
        for (auto it = clients.begin(); it != clients.end();) {
            if ((*it)->is_dead()) {
                to_remove.push_back(*it);
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (auto& client: to_remove) {
        if (client) {
            if (client->is_alive()) {
                client->join();
            }
        }
    }
}

size_t ServerProtectedClients::size() {
    std::lock_guard<std::mutex> lock(m);
    return clients.size();
}

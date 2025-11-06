#include "protected_clients.h"

#include "client_handler.h"

ServerProtectedClients::ServerProtectedClients(): clients() {}


void ServerProtectedClients::broadcast(std::shared_ptr<ServerToClientCmd_Server> cmd) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client: clients) {
        client->send_message(cmd);
    }
}

void ServerProtectedClients::add_client(ServerClientHandler* client) {
    std::lock_guard<std::mutex> lock(m);
    clients.push_back(client);
}

void ServerProtectedClients::stop_all_and_delete() {
    std::lock_guard<std::mutex> lock(m);
    for (auto* client: clients) {
        if (client == nullptr)
            continue;
        client->stop();
        client->join();
        delete client;
    }
}

void ServerProtectedClients::stop_and_delete_dead() {
    std::vector<ServerClientHandler*> to_remove;
    {
        std::lock_guard<std::mutex> lock(m);
        for (auto it = clients.begin(); it != clients.end();) {
            ServerClientHandler* c = *it;
            if (c->is_dead()) {
                to_remove.push_back(c);
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
    }
    for (ServerClientHandler* c: to_remove) {
        if (c == nullptr)
            continue;
        if (c->is_alive()) {
            c->join();
        }
        delete c;
    }
}

size_t ServerProtectedClients::size() {
    std::lock_guard<std::mutex> lock(m);
    return clients.size();
}
